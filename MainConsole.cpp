#include "MainConsole.h"
#include <iostream>
#include <windows.h>
#include "ConsoleManager.h"
#include "ScreenConsole.h"

MainConsole::MainConsole() : AConsole("MAIN_CONSOLE"), menuShown(false) {}

void MainConsole::onEnabled() {
    display();
}

void MainConsole::display() {
    //if (!menuShown) {
    //    menu();
    //    menuShown = true;
    //}
    //// Remove the for loop from here
    //enter();
    //system("cls");  // Clear the console
    menu();
    for (const auto& cmd : commandHist) {
        std::cout << "Enter a command: " << cmd << std::endl;
        std::cout << "You entered: " << cmd << std::endl;
    }
    enter();
}

void MainConsole::redisplay() {
    system("cls");  // Clear the console
    menu();
    for (const auto& cmd : commandHist) {
        std::cout << "Enter a command: " << cmd << std::endl;
        std::cout << "You entered: " << cmd << std::endl;
    }
    // Remove the enter() call from here
}

void MainConsole::process() {
    std::string command;
    std::getline(std::cin, command);

    if (!command.empty()) {
        commandHist.push_back(command);
    }

    std::cout << "You entered: " << command << std::endl;

    if (command == "initialize") {
        std::cout << "Initialize command recognized. Doing something." << std::endl;
    }
    else if (command == "screen") {
        std::cout << "Screen command recognized. Doing something." << std::endl;
    }
    else if (command.rfind("screen -s ", 0) == 0) {
        std::string processName = command.substr(10);
        auto newScreen = std::make_shared<ScreenConsole>(processName, 1, 100);
        ConsoleManager::getInstance()->registerScreen(newScreen);
        return;  // Don't display menu after switching
    }
    else if (command.rfind("screen -r ", 0) == 0) {
        std::string processName = command.substr(10);
        if (ConsoleManager::getInstance()->activeScreens.find(processName) !=
            ConsoleManager::getInstance()->activeScreens.end()) {
            auto screenConsole = std::make_shared<ScreenConsole>(processName, 1, 100);
            ConsoleManager::getInstance()->switchConsole(screenConsole);
        }
        else {
            std::cout << "No active screen found for: " << processName << std::endl;
        }
        return;  // Don't display menu after switching
    }
    else if (command == "exit") {
        std::cout << "Exit command recognized. Preparing to exit." << std::endl;
        ConsoleManager::getInstance()->exitApplication();
        return;
    }
    else if (command == "history") {
        showHistory();
    }
    else if (!command.empty()) {
        std::cout << "Command not recognized: " << command << std::endl;
    }

    //enter();
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