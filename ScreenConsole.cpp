#include "ScreenConsole.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>
#include <sstream>
#include "ConsoleManager.h"

ScreenConsole::ScreenConsole(const std::string& processName, int currentLine, int totalLines)
    : AConsole("PLACEHOLDER_CONSOLE"), processName(processName), currentLine(currentLine), totalLines(totalLines)
{
    timestamp = getCurrentTimestamp();
}

void ScreenConsole::onEnabled() {
    system("cls");  // Clear the console
    this->display();
}


void ScreenConsole::display() {
    std::cout << "Process Name: " << processName << std::endl;
    std::cout << "Current Line: " << currentLine << " / Total Lines: " << totalLines << std::endl;
    std::cout << "Timestamp: " << timestamp << std::endl;
    std::cout << "Enter a command or type 'exit' to return to the main menu." << std::endl;
}

void ScreenConsole::process() {
    std::string command;

    std::cout << '[' << processName << "] $ ";
    std::getline(std::cin, command);

    if (!command.empty()) {
        commandHist.push_back(command);
    }

    if (command == "exit") {
        ConsoleManager::getInstance()->returnToPreviousConsole();
        return;
    }
    else if (command.empty()) {
        std::cout << "No command entered." << std::endl;
    }
    else {
        std::cout << "Executing command: " << command << std::endl;
        // Here you would typically process the command
        std::cout << "Command executed." << std::endl;
    }

    display();  // Redisplay the console after processing
}

std::string ScreenConsole::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm = *std::localtime(&now_c);

    std::ostringstream oss;
    oss << std::put_time(&now_tm, "%m/%d/%Y, %I:%M:%S %p");
    return oss.str();
}
