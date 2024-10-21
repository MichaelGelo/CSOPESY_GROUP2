#include "ConsoleManager.h"
#include <iostream>

int main() {
    try {
        // Init the ConsoleManager
        ConsoleManager::initialize();

        // ConsoleManager
        ConsoleManager* manager = ConsoleManager::getInstance();

        // Main loop
        while (manager->isRunning()) {
            // Draw the current console
            manager->drawConsole();

            // Process input and update state
            manager->process();
        }

        // Clean up
        ConsoleManager::destroy();
    }
    catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}