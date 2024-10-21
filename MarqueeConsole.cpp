#include "MarqueeConsole.h"
#include <iostream>
#include <thread>
#include <chrono>

// CHANGE THIS CODE!!!!!!!
// CHANGE THIS CODE!!!!!!!
// CHANGE THIS CODE!!!!!!!

MarqueeConsole::MarqueeConsole() : AConsole("MARQUEE_CONSOLE") {}

void MarqueeConsole::onEnabled() {
    std::cout << "Marquee Console Enabled!" << std::endl;
}

void MarqueeConsole::display() {
    std::cout << "Displaying Marquee... (This will animate in the future)" << std::endl;

    std::string message = "Hello, marquee";
    for (int i = 0; i < 10; ++i) {
        std::cout << "\r" << message;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        message = " " + message;  
    }
    std::cout << std::endl;
}

void MarqueeConsole::process() {
    std::cout << "Press 'exit' to return to the main menu: ";
    std::string command;
    std::cin >> command;
    if (command == "exit") {
        std::cout << "Returning to Main Console..." << std::endl;
      
    }
    else {
        std::cout << "Unrecognized command: " << command << std::endl;
    }
}
