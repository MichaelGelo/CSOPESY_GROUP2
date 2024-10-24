#include "ScreenConsole.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "ConsoleManager.h"
#include "Process.h"

ScreenConsole::ScreenConsole(const std::string& processName, int pid, int core, int currentLine, int totalLines, std::vector<std::shared_ptr<Process>>& processes)
    : AConsole(processName), processName(processName), pid(pid), core(core), currentLine(currentLine), totalLines(totalLines), processes(processes) // Initialize processes here
{
    auto newProcess = std::make_shared<Process>(pid, processName, core, totalLines);
    this->processes.push_back(newProcess);  // Now you're correctly pushing to the member processes

    timestamp = getCurrentTimestamp();
    lastCommand = ""; // Initialize lastCommand
}

void ScreenConsole::onEnabled() {
    system("cls");
    this->display();
}

void ScreenConsole::display() {
    system("cls");
    std::cout << "Process: " << processName << std::endl;
    std::cout << "ID: " << pid << std::endl;
    std::cout << "\nCurrent Instruction Line: " << currentLine << std::endl;
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
            std::cout << "Command not recognized: (empty command)" << std::endl;
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
        else{
            
            std::cout << "Command not recognized: " << command << std::endl;
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

    if (currentLine >= totalLines) {
        std::cout << "Finished!" << std::endl;
    }
    else {
        std::cout << "\nCurrent Instruction Line: " << currentLine << std::endl;
        std::cout << "Lines of Code: " << totalLines << "\n" << std::endl;
    }
}