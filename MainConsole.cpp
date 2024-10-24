#include "MainConsole.h"
#include <iostream>
#include <windows.h>
#include "ConsoleManager.h"
#include "ScreenConsole.h"
#include "MarqueeConsole.h"
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



MainConsole::MainConsole(): AConsole("MAIN_CONSOLE"),menuShown(false),isInitialized(false),schedulerInstance(nullptr),cpuCycleCounter(nullptr),isCPURunning(false) {}

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
            || (cmd.rfind("Please initialize", 0) == 0) || (cmd.rfind("Marquee command", 0) == 0)) {
            std::cout << cmd << std::endl;
        }
        else {
            std::cout << "You entered: " << cmd << std::endl;
        }
    }

    enter();
}

std::map<std::string, std::string> readConfigFile(const std::string& filename) {
    std::map<std::string, std::string> config;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cout << "Error: Could not open configuration file: " << filename << std::endl;
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

void MainConsole::createProcess(std::string processName){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(this->config.minIns, this->config.maxIns);
    int randomLines = dis(gen);

    auto newScreen = std::make_shared<ScreenConsole>(processName, 1, nextPid, 0, randomLines, processes);
    ConsoleManager::getInstance()->registerScreen(newScreen);
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
            if (cpuCycleCounter && isCPURunning) {
                std::cout << "Stopping CPU cycle counter. Final cycle count: "
                    << cpuCycleCounter->getCurrentCycle() << std::endl;
                cpuCycleCounter->stopClock();
                isCPURunning = false;
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
                std::cout << "No valid configuration found." << std::endl;
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
                cpuCycleCounter = std::make_unique<CPUCycle>();
                cpuCycleCounter->startClock();
                isCPURunning = true;
                std::cout << "CPU cycle counter started." << std::endl;

                // Create the Scheduler object using the stored configuration
                schedulerInstance = std::make_shared<Scheduler>(  // Changed to make_shared since schedulerInstance is shared_ptr
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
            std::cout << "Please initialize the cores first by using the 'initialize' command.\n";
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

    ////////////////// EDIT

    else if (command == "screen") {
        captureAndStoreOutput([]() {
            std::cout << "Screen command recognized. Doing something." << std::endl;
            });
    }
    else if (command == "scheduler-test") {
        captureAndStoreOutput([this]() {
            if (!isInitialized) {
                std::cout << "Scheduler is not initialized. Please run the initialize command first." << std::endl;
                return;
            }

            std::cout << "Scheduler test command recognized. Running the test." << std::endl;

            // "schedulerInstance->" ito na 'yung gagamitin mo with anything that has to do with the config.txt values
            // schedulerInstance->function();

            // Display to check if naka-store na siya or smthn
            schedulerInstance->displayConfiguration();
            });
        display();
    }
    else if (command == "scheduler-stop") {
        captureAndStoreOutput([]() {
            std::cout << "Scheduler stop command recognized. Doing something." << std::endl;
            });
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
            createProcess(processName);
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
        display();
        return;
    }
    else if (command == "history") {
        showHistory();
    }
    else if (!command.empty()) {
        captureAndStoreOutput([command]() {
            std::cout << "Command not recognized: " << command << std::endl;
            });
        display();
    }
}

void MainConsole::menu() const {
    color(13);
    std::cout << R"(
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
    )" << std::endl;
    color(7);
}

void MainConsole::enter() const {
    color(7);
    std::cout << "Enter a command: ";
}

void MainConsole::color(int n) const {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), n);
}

void MainConsole::showHistory() const {
    std::cout << "Command History:" << std::endl;
    for (size_t i = 0; i < commandHist.size(); ++i) {
        std::cout << i + 1 << ": " << commandHist[i] << std::endl;
    }
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

    std::cout << "-----------------------------------------------------------------\n";
    std::cout << "Running Processes:\n";

    // Display running processes (READY, RUNNING, WAITING states)
    for (const auto& process : processes) {
        Process::ProcessState state = process->getState();
        if (state == Process::READY || state == Process::RUNNING || state == Process::WAITING) {
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

void MainConsole::saveProcessReport() const {
    std::ofstream logFile("csopesy-log.txt");
    if (!logFile) {
        std::cout << "Error: Could not open log file." << std::endl;
        return;
    }

    // Get current time for the log entry
    time_t now = time(0);
    char* dt = ctime(&now);

    // Write header with timestamp
    logFile << "\n=================================================================\n";
    logFile << "Process Report - Generated at: " << dt;
    logFile << "=================================================================\n";

    // Redirect cout to the file temporarily
    std::streambuf* oldCoutBuffer = std::cout.rdbuf();
    std::cout.rdbuf(logFile.rdbuf());

    // Use existing display function
    displayProcessStatus();

    // Restore cout
    std::cout.rdbuf(oldCoutBuffer);
    logFile.close();
}