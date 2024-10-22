#include "MainConsole.h"
#include <iostream>
#include <windows.h>
#include "ConsoleManager.h"
#include "ScreenConsole.h"
#include <sstream>

MainConsole::MainConsole() : AConsole("MAIN_CONSOLE"), menuShown(false) {}

void MainConsole::onEnabled() {
    display();
}

void MainConsole::display() {
    system("cls");
    menu();

    for (const auto& cmd : commandHist) {
        // just for aesthetic
        if ((cmd.rfind("Active Screens", 0) == 0) || (cmd.rfind("Command not recognized", 0) == 0) || (cmd.rfind("No active screens.", 0) == 0)
            || (cmd.rfind("No active screen found for:", 0) == 0) || (cmd.rfind("Screen name", 0) == 0)) {
            std::cout << cmd << std::endl;
        }
        else {
            std::cout << "You entered: " << cmd << std::endl;
        }
    }

    enter();
}


void MainConsole::process() {
    std::string command;
    std::getline(std::cin, command);

    if (!command.empty()) {
        commandHist.push_back(command);
    }

    if (command == "initialize") {
        captureAndStoreOutput([]() {
            std::cout << "Initialize command recognized. Doing something." << std::endl;
            });
    }
    else if (command == "screen") {
        captureAndStoreOutput([]() {
            std::cout << "Screen command recognized. Doing something." << std::endl;
            });
    }
    else if (command == "scheduler-test") {
        captureAndStoreOutput([]() {
            std::cout << "Scheduler test command recognized. Doing something." << std::endl;
            });
    }
    else if (command == "scheduler-stop") {
        captureAndStoreOutput([]() {
            std::cout << "Scheduler stop command recognized. Doing something." << std::endl;
            });
    }
    else if (command == "report-util") {
        captureAndStoreOutput([]() {
            std::cout << "Report util command recognized. Doing something." << std::endl;
            });
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
            auto newScreen = std::make_shared<ScreenConsole>(processName, 1, 100);
            ConsoleManager::getInstance()->registerScreen(newScreen);
            return;  // Don't display menu after switching
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
                std::cout << "No active screen found for: " << processName << std::endl;
                });
            display();
        }
        return;  // Don't display menu after switching
    }
    else if (command == "screen -ls") {
        captureAndStoreOutput([this]() {
            ConsoleManager::getInstance()->showActiveScreens();
            });
        display(); 
        return;  // Don't display menu after listing screens
    }

    else if (command == "exit") {
        captureAndStoreOutput([]() {
            std::cout << "Exit command recognized. Preparing to exit." << std::endl;
            });
        ConsoleManager::getInstance()->exitApplication();
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
