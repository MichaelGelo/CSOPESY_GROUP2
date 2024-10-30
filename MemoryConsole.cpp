#include "MemoryConsole.h"
#include "Design.h"
#include <iostream>

// CHANGE THIS CODE!!!!!!!
// CHANGE THIS CODE!!!!!!!
// CHANGE THIS CODE!!!!!!!

MemoryConsole::MemoryConsole() : AConsole("MEMORY_CONSOLE") {}

void MemoryConsole::onEnabled() {
    std::cout << "Memory Console Enabled!" << std::endl;
}

void MemoryConsole::display() {
    std::cout << "Displaying Memory Console Information..." << std::endl;
    
}

void MemoryConsole::process() {
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
