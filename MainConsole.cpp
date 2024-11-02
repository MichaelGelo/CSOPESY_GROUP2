#include "MainConsole.h"
#include <iostream>
#include <windows.h>
#include "ConsoleManager.h"
#include "ScreenConsole.h"
#include "MarqueeConsole.h"
#include "CPUCore.h"
#include "CPUCycle.h"
#include "ICommand.h"
#include <sstream>
#include <fstream>
#include <ctime>
#include <string>
#include <map> 
#include "Scheduler.h"
#include <random>
#include <condition_variable>
#include <functional>
#include <iomanip>
#include <memory>
#include "Design.h"

MainConsole::MainConsole() : AConsole("MAIN_CONSOLE"), menuShown(false), isInitialized(false), schedulerInstance(nullptr), cpuCycleCounter(nullptr), isCPURunning(false) {}

void MainConsole::onEnabled() {
    display();
}
void MainConsole::display() {
    system("cls");
    menu();

    for (const auto& cmd : commandHist) {
        // just for aesthetic
        if ((cmd.rfind("Active Screens", 0) == 0) || (cmd.rfind("Command not recognized", 0) == 0) || (cmd.rfind("No active screens.", 0) == 0)
            || (cmd.rfind("Process <", 0) == 0) || (cmd.rfind("Screen name", 0) == 0) || (cmd.rfind("----------------", 0) == 0)
            || (cmd.rfind("Generating process utilization report...", 0) == 0) || (cmd.rfind("Initialize command recognized", 0) == 0)
            || (cmd.rfind("Please initialize", 0) == 0) || (cmd.rfind("Marquee command", 0) == 0) || (cmd.rfind("\033[31m", 0) == 0) || (cmd.rfind("\033[32m", 0) == 0)
            || (cmd.rfind("Scheduler test stopped", 0) == 0) || (cmd.rfind("Starting scheduler test...", 0) == 0)) {
            std::cout << cmd << std::endl;
        }
        else {
            std::cout << "Enter a command: " << cmd << std::endl;
        }
    }

    enter();
}

std::map<std::string, std::string> readConfigFile(const std::string& filename) {
    std::map<std::string, std::string> config;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cout << RED << "Error: Could not open configuration file: " << filename << RESET << std::endl;
        return config;
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find(' ');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            config[key] = value;
        }
    }

    file.close();
    return config;
}

void MainConsole::createProcess(std::string processName) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(this->config.minIns, this->config.maxIns);
    int randomLines = dis(gen);

    // Create ScreenConsole, which in turn creates the Process
    auto newScreen = std::make_shared<ScreenConsole>(processName, nextPid++, 1, randomLines, processes);
    ConsoleManager::getInstance()->registerScreen(newScreen);

    // Add the internally created Process to the Scheduler
    schedulerInstance->addToRQ(newScreen->getProcess());
}



void MainConsole::process() {
    std::string command;
    std::getline(std::cin, command);

    if (!command.empty()) {
        commandHist.push_back(command);
    }
    if (command == "exit") {
        captureAndStoreOutput([this]() {
            std::cout << "Exit command recognized. Preparing to exit." << std::endl;
            if (isCPURunning) {
                isCPURunning = false;
                cpuCycle.stopClock();
            }
            });
        ConsoleManager::getInstance()->exitApplication();
        return;
    }
    else if (command == "initialize") {
        captureAndStoreOutput([this]() {
            std::cout << "Initialize command recognized. Reading configuration..." << std::endl;

            auto configFile = readConfigFile("config.txt");  // Changed variable name to configFile

            if (configFile.empty()) {
                std::cout << RED << "No valid configuration found." << RESET << std::endl;
            }
            else {
                std::cout << "Configuration loaded:" << std::endl;

                // Store values in the config member variable (not local config)
                this->config.numCpu = std::stoi(configFile["num-cpu"]);
                this->config.scheduler = configFile["scheduler"];
                this->config.quantumCycles = std::stoi(configFile["quantum-cycles"]);
                this->config.batchProcessFreq = std::stoi(configFile["batch-process-freq"]);
                this->config.minIns = std::stoi(configFile["min-ins"]);
                this->config.maxIns = std::stoi(configFile["max-ins"]);
                this->config.delayPerExec = std::stoi(configFile["delay-per-exec"]);

                // Initialize the CPU cycle counter
                cpuCycle.startClock();
                isCPURunning = true;

                // Create the Scheduler object using the stored configuration
                schedulerInstance = std::make_shared<Scheduler>(  // Changed to make_shared since schedulerInstance is shared_ptr
                    cpuCycle,           // added cpuCycle
                    this->config.numCpu,
                    this->config.scheduler,
                    this->config.quantumCycles,
                    this->config.batchProcessFreq,
                    this->config.minIns,
                    this->config.maxIns,
                    this->config.delayPerExec
                );

                schedulerInstance->displayConfiguration();  // Changed schedulerObject to schedulerInstance
                isInitialized = true;
            }
            });

        display();
    }

    else if (!isInitialized) {
        captureAndStoreOutput([]() {
            std::cout << RED << "Please initialize the cores first by using the 'initialize' command.\n" << RESET;
            });
        display();
        return;
    }

    ////////////////// EDIT

    else if (command == "marquee") {
        captureAndStoreOutput([]() {
            std::cout << "Marquee command recognized. Launching MarqueeConsole..." << std::endl;
            });

        MarqueeConsole marqueeApp;
        marqueeApp.run();

        display();
    }

    else if (command == "nvidia-smi") {
        captureAndStoreOutput([]() {
            std::cout << "Command recognized. Doing something." << std::endl;
            });
    }


    else if (command == "screen") {
        captureAndStoreOutput([]() {
            std::cout << "Screen command recognized. Doing something." << std::endl;
            });
    }
    if (command == "scheduler-test") {
        if (!isInitialized) {
            captureAndStoreOutput([this]() {
                std::cout << RED << "Scheduler is not initialized. Please run the initialize command first." << RESET << std::endl;
                });
        }
        else {
            captureAndStoreOutput([this]() {
                std::cout << GREEN << "Starting scheduler test...\n" << "Creating Dummy Processes..." << RESET << std::endl;
                schedulerTest();
                });
        }
        display();
        return;
    }
    else if (command == "scheduler-stop") {
        captureAndStoreOutput([this]() {
            if (!isInitialized) {
                std::cout << "Scheduler is not initialized. Please run the initialize command first." << std::endl;
                return;
            }
            schedulerStop();
            });
        display();
    }
    else if (command == "report-util") {
        captureAndStoreOutput([this]() {
            std::cout << "Generating process utilization report...\n";
            saveProcessReport();
            std::cout << "Report has been saved to csopesy-log.txt\n";
            });
        display();
    }
    else if (command == "clear") {
        system("cls");
        menu();
        enter();
    }
    else if (command.rfind("screen -s ", 0) == 0) {
        std::string processName = command.substr(10);
        if (ConsoleManager::getInstance()->screenExists(processName)) {
            captureAndStoreOutput([processName]() {
                std::cout << "Screen name \"" << processName << "\" already exists. Please use a different name." << std::endl;
                });
            display();
        }
        else {
            // Create process with default core=1 and maxLines=100
            // Create process with default core=1 and maxLines=100
            createProcess(processName);
            ConsoleManager::getInstance()->switchConsole(processName);
            return;
        }
    }
    else if (command.rfind("screen -r ", 0) == 0) {
        std::string processName = command.substr(10);
        if (ConsoleManager::getInstance()->activeScreens.find(processName) !=
            ConsoleManager::getInstance()->activeScreens.end()) {
            auto screenConsole = ConsoleManager::getInstance()->activeScreens[processName];
            ConsoleManager::getInstance()->switchConsole(screenConsole);
        }
        else {
            captureAndStoreOutput([processName]() {
                std::cout << "Process <" << processName << "> not found." << std::endl;
                });
            display();
        }
        return;  // Don't display menu after switching
    }
    else if (command == "screen -ls") {
        captureAndStoreOutput([this]() {
            displayProcessStatus();
            });
        if (!commandHist.empty()) {
            screenListHist.push_back(commandHist.back());
        }
        display();
        return;
    }
    else if (command == "history") {
        showHistory();
    }
    else if (!command.empty()) {
    commandHist.push_back(command);
    }
 else {
     // If empty command (just Enter), simply redisplay the prompt
     enter();
     return;
    }
    display();
}

void MainConsole::schedulerTest() {
    if (!isSchedulerTestRunning) {
        isSchedulerTestRunning = true;
        testThread = std::thread(&MainConsole::runSchedulerTest, this);
        testThread.detach();
    }
    else {
        std::cout << "Scheduler test is already running." << std::endl;
    }
    
}

void MainConsole::schedulerStop() {
    if (isSchedulerTestRunning) {
        isSchedulerTestRunning = false;
        std::cout << GREEN << "Stopping Creating Dummy Processes...\n" << "Scheduler test stopped." << RESET << std::endl;
    }
    else {
        std::cout << "Scheduler test is not running." << std::endl;
    }
}

void MainConsole::runSchedulerTest() {
    //display();
    while (isSchedulerTestRunning && isCPURunning) {
        //std::cout << "Scheduler test running..." << std::endl;

        std::unique_lock<std::mutex> lock(cpuCycle.mtx);
        cpuCycle.cv.wait(lock);

        int currentCycle = cpuCycle.getCurrentCycle();
        
        if (currentCycle % config.batchProcessFreq == 0) {
            std::string processName = "process" + std::to_string(currentCycle);
            while (ConsoleManager::getInstance()->screenExists(processName)) processName += "X";
            createProcess(processName);
        }
    }
}
void MainConsole::menu() const {

    std::cout << YELLOW << R"(
===============================================================
 _______  _______  _______  _______  _______  _______  **   ** 
|       ||       ||       ||       ||       ||       ||  | |  |
|       ||  _____||   *   ||    *  ||    ___||  _____||  |_|  |
|       || |_____ |  | |  ||   |_| ||   |___ | |_____ |       |
|      *||*____  ||  |_|  ||    ___||    ___||_____  ||_     _|
|     |_  _____| ||       ||   |    |   |___  _____| |  |   |  
|_______||_______||_______||___|    |_______||_______|  |___|  
AGUSTINES    --   DEPASUCAT     --     ESTEBAN     --  PADILLA
HELLO, WELCOME TO  GROUP 2'S CSOPESY COMMANDLINE!
TYPE 'exit' TO QUIT
===============================================================
    )" << RESET << std::endl;
}

void MainConsole::enter() const {

    std::cout << "Enter a command: ";

}



void MainConsole::showHistory() const {
    std::cout << "Command History:" << std::endl;
    for (size_t i = 0; i < commandHist.size(); ++i) {
        std::cout << i + 1 << ": " << commandHist[i] << std::endl;
    }
}


void MainConsole::clear() {
    //system("cls");
}

void MainConsole::captureAndStoreOutput(std::function<void()> func) {
    std::ostringstream outputStream;
    std::streambuf* oldCoutBuffer = std::cout.rdbuf(); // Save the old buffer
    std::cout.rdbuf(outputStream.rdbuf()); // Redirect cout to outputStream

    func(); // Call the passed function

    std::cout.rdbuf(oldCoutBuffer); // Restore the original cout buffer

    // Add the captured output to commandHist
    commandHist.push_back(outputStream.str());
}

void MainConsole::displayProcessStatus() const {
    bool hasRunningProcess = false;
    bool hasFinishedProcess = false;

    int unfinishedProcesses = 0;
    for (const auto& process : processes) {
        Process::ProcessState state = process->getState();
        if (state == Process::RUNNING) {
            unfinishedProcesses++;
        }
    }
    float core_used = unfinishedProcesses < config.numCpu ? unfinishedProcesses : config.numCpu;
    float cpu_utilization = (core_used / (float)config.numCpu) * 100;
    float core_available = config.numCpu - core_used;

    std::cout << GREEN << "\nCPU Utilization: " << std::fixed << std::setprecision(2)<< cpu_utilization << "%" << std::endl;
    std::cout << "Core Used: " << static_cast<int>(core_used) << std::endl;
    std::cout << "Core Available: " << static_cast<int>(core_available) << "\n" << RESET << std::endl;
    std::cout << "Running Processes:\n";

    std::cout << "-----------------------------------------------------------------\n";

    // Display running processes (READY, RUNNING, WAITING states)
    for (const auto& process : processes) {
        Process::ProcessState state = process->getState();
        if (state == Process::RUNNING) {
            process->displayProcessInfo();
            hasRunningProcess = true;
        }
    }

    if (!hasRunningProcess) {
        std::cout << "There are no active processes.\n";
    }

    std::cout << "\nFinished Processes:\n";

    // Display finished processes (FINISHED state)
    for (const auto& process : processes) {
        if (process->getState() == Process::FINISHED || process->hasFinished()) {
            process->displayProcessInfo();
            hasFinishedProcess = true;
        }
    }

    if (!hasFinishedProcess) {
        std::cout << "There are no completed processes.\n";
    }

    std::cout << "-----------------------------------------------------------------\n";
}


//void MainConsole::saveProcessReport() const {
//    std::ofstream logFile("csopesy-log.txt");
//    if (!logFile) {
//        std::cout << RED << "Error: Could not open log file." << RESET << std::endl;
//        return;
//    }
//
//    // Get current time for the log entry
//    time_t now = time(0);
//    char* dt = ctime(&now);
//
//    // Write header with timestamp
//    logFile << "\n=================================================================\n";
//    logFile << "Process Report - Generated at: " << dt;
//    logFile << "=================================================================\n";
//
//    // Redirect cout to the file temporarily
//    std::streambuf* oldCoutBuffer = std::cout.rdbuf();
//    std::cout.rdbuf(logFile.rdbuf());
//
//    // Use existing display function
//    displayProcessStatus();
//
//    // Restore cout
//    std::cout.rdbuf(oldCoutBuffer);
//    logFile.close();
//}

void MainConsole::saveProcessReport() const {
    if (screenListHist.empty()) {
        return;
    }

    std::ofstream logFile("csopesy-log.txt");
    if (!logFile) {
        std::cout << RED << "Error: Could not open log file." << RESET << std::endl;
        return;
    }

    time_t now = time(0);
    char* dt = ctime(&now);

    logFile << "\n=================================================================\n";
    logFile << "Process Report - Generated at: " << dt;
    logFile << "=================================================================\n";
    logFile << screenListHist.back();  // Write the last screen -ls output
    logFile.close();
}