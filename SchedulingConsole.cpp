#include "SchedulingConsole.h"
#include "Design.h"
#include <iostream>

// CHANGE THIS CODE!!!!!!!
// CHANGE THIS CODE!!!!!!!
// CHANGE THIS CODE!!!!!!!
SchedulingConsole::SchedulingConsole() : AConsole("SCHEDULING_CONSOLE") {}

void SchedulingConsole::onEnabled() {
    std::cout << "Scheduling Console Enabled!" << std::endl;
}

void SchedulingConsole::display() {
    std::cout << "Displaying Scheduling Console Information..." << std::endl;
    
}

void SchedulingConsole::process() {
    std::cout << "Press 'exit' to return to the main menu: ";
    std::string command;
    std::cin >> command;
    if (command == "exit") {
        std::cout << "Returning to Main Console..." << std::endl;
      
    }
    else {
        std::cout << RED << "Unrecognized command: " << command << RESET << std::endl;
    }
}
