#include "ScreenConsole.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "ConsoleManager.h"

ScreenConsole::ScreenConsole(const std::string& processName, int currentLine, int totalLines)
    : AConsole(processName), processName(processName), currentLine(currentLine), totalLines(totalLines)
{
    timestamp = getCurrentTimestamp();
    lastCommand = ""; // Initialize lastCommand
}

void ScreenConsole::onEnabled() {
    system("cls");
    this->display();
}

void ScreenConsole::display() {
    system("cls");
    std::cout << "Process Name: " << processName << std::endl;
    std::cout << "Current Line: " << currentLine << " / Total Lines: " << totalLines << std::endl;
    std::cout << "Timestamp: " << timestamp << std::endl;
    std::cout << "\nEnter a command or type 'exit' to return to the main menu.\n" << std::endl;
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
            std::cout << "No command entered." << std::endl;
            continue; 
        }


        lastCommand = command;

        if (command == "exit") {
            ConsoleManager::getInstance()->returnToPreviousConsole();
            break; 
        }

 
        std::cout << "Executing command: " << command << std::endl;
        
        std::cout << "Command executed." << std::endl;

        
        display();
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
