#include "MainConsole.h"
#include <iostream>
#include <cstdlib>
#include <windows.h>

MainConsole::MainConsole() : AConsole("MAIN_CONSOLE"), menuShown(false) {}

void MainConsole::onEnabled()
{
    clear();
    if (!menuShown) {
        display();
    }
}

void MainConsole::display()
{
    if (!menuShown) {
        menu();
        menuShown = true;
    }
}

void MainConsole::process()
{
    std::string command;
    enter();
    std::getline(std::cin, command);

    // Store the command in history
    if (!command.empty()) {
        commandHist.push_back(command);
    }

    if (command == "initialize") {
        std::cout << "Initialize command recognized. Doing something." << std::endl;
    }
    else if (command == "screen") {
        std::cout << "Screen command recognized. Doing something." << std::endl;
    }
    else if (command == "scheduler-test") {
        std::cout << "Scheduler test command recognized. Doing something." << std::endl;
    }
    else if (command == "scheduler-stop") {
        std::cout << "Scheduler stop command recognized. Doing something." << std::endl;
    }
    else if (command == "report-util") {
        std::cout << "Report util command recognized. Doing something." << std::endl;
    }
    else if (command == "clear") {
        clear();
        menuShown = false;  // Reset the flag so the menu will be shown after clearing
        display();  // Show the menu again after clearing
    }
    else if (command == "exit") {
        std::cout << "Exit command recognized. Preparing to exit." << std::endl;
    }
    else if (command == "history") {
        showHistory();
    }
    else if (!command.empty()) {
        std::cout << "Command not accepted: " << command << std::endl;
    }
}

void MainConsole::menu() const
{
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
TYPE 'exit' TO QUIT, 'clear' TO CLEAR THE SCREEN
===============================================================
        )" << std::endl;
}

void MainConsole::enter() const
{
    color(7);
    std::cout << "Enter a command: ";
}

void MainConsole::clear() const
{
    system("cls");
}

void MainConsole::color(int n) const
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), n);
}

void MainConsole::showHistory() const
{
    std::cout << "Command History:" << std::endl;
    for (size_t i = 0; i < commandHist.size(); ++i) {
        std::cout << i + 1 << ": " << commandHist[i] << std::endl;
    }
}
