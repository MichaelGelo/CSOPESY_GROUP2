#include "ConsoleManager.h"
#include <iostream>

int main() {
    ConsoleManager::initialize();

    while (ConsoleManager::getInstance()->isRunning()) {
        ConsoleManager::getInstance()->drawConsole();
        ConsoleManager::getInstance()->process();
    }

    ConsoleManager::destroy();
    return 0;
}
