#include "ScreenConsole.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "ConsoleManager.h"
#include "Design.h"
#include "Process.h"

ScreenConsole::ScreenConsole(const std::string& processName, int pid, int core, int totalLines, int memoryRequirement, std::vector<std::shared_ptr<Process>>& processes)
    : AConsole(processName), processName(processName), pid(pid), core(core), totalLines(totalLines), processes(processes)
{
    processInstance = std::make_shared<Process>(pid, processName, core, totalLines, memoryRequirement);
    processInstance->generateCommands(); 

    // Add the new process instance to the vector of processes
    this->processes.push_back(processInstance);

    timestamp = getCurrentTimestamp(); // Initialize timestamp
    lastCommand = ""; // Initialize last command
}


std::shared_ptr<Process> ScreenConsole::getProcess() const {
    return processInstance;
}

void ScreenConsole::onEnabled() {
    system("cls");
    this->display();
}

void ScreenConsole::display() {
    system("cls");
    std::cout << "Process: " << processName << std::endl;
    std::cout << "ID: " << pid << std::endl;
    std::cout << "\nCurrent Instruction Line: " << processInstance->getCurLines() << std::endl;
    std::cout << "Lines of Code: " << totalLines << std::endl;
    std::cout << "Timestamp: " << timestamp << std::endl;
    std::cout << "\nType 'exit' to return to the main menu.\n" << std::endl;

    // Display last command
    if (!lastCommand.empty()) {
        std::cout << '[' << processName << "] $ " << lastCommand << std::endl;
    }
}

void ScreenConsole::process() {
    std::string command;

    while (true) {
        std::cout << '[' << processName << "] $ ";
        std::getline(std::cin, command);

        if (command.empty()) {
            std::cout << RED << "Command not recognized: (empty command)" << RESET << std::endl;
            continue;
        }

        lastCommand = command;

        if (command == "exit") {
            ConsoleManager::getInstance()->returnToPreviousConsole();
            break;
        }
        else if (command == "process-smi") {
            handleProcessSmi();
        }
        else {
            std::cout << RED << "Command not recognized: " << command << RESET << std::endl;
        }
    }
}

std::string ScreenConsole::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm = *std::localtime(&now_c);

    std::ostringstream oss;
    oss << std::put_time(&now_tm, "%m/%d/%Y, %I:%M:%S %p");
    return oss.str();
}

void ScreenConsole::handleProcessSmi() {
    std::cout << "\nProcess: " << processName << std::endl;
    std::cout << "ID: " << pid << std::endl;

    if (processInstance->getCurLines() >= totalLines) {
        std::cout << "Finished!" << std::endl;
    }
    else {
        std::cout << "\nCurrent Instruction Line: " << processInstance->getCurLines() << std::endl;
        std::cout << "Lines of Code: " << totalLines << "\n" << std::endl;
    }
}
