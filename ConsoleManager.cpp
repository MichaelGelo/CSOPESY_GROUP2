#include "ConsoleManager.h"
#include "MainConsole.h"
#include "MarqueeConsole.h"
#include "SchedulingConsole.h"
#include "MemoryConsole.h"
#include "ScreenConsole.h"
#include <iostream>
ConsoleManager* ConsoleManager::sharedInstance = nullptr;

ConsoleManager::ConsoleManager() {
    this->running = true;
    this->consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    this->consoleTable[MAIN_CONSOLE] = std::make_shared<MainConsole>();
    this->consoleTable[MARQUEE_CONSOLE] = std::make_shared<MarqueeConsole>();
    this->consoleTable[SCHEDULING_CONSOLE] = std::make_shared<SchedulingConsole>();
    this->consoleTable[MEMORY_CONSOLE] = std::make_shared<MemoryConsole>();
    this->switchConsole(MAIN_CONSOLE);
}

ConsoleManager* ConsoleManager::getInstance() {
    if (sharedInstance == nullptr) {
        sharedInstance = new ConsoleManager();
    }
    return sharedInstance;
}

void ConsoleManager::initialize() {
    getInstance();
}

void ConsoleManager::destroy() {
    delete sharedInstance;
    sharedInstance = nullptr;
}

void ConsoleManager::drawConsole() const {
    if (currentConsole) {
        //currentConsole->display();
    }
}

void ConsoleManager::process() const {
    if (currentConsole) {
        currentConsole->process();
    }
}

void ConsoleManager::switchConsole(const std::string& consoleName) {
    auto it = consoleTable.find(consoleName);
    if (it != consoleTable.end()) {
        previousConsole = currentConsole;
        currentConsole = it->second;
        system("cls");  // Clear the console
        currentConsole->onEnabled();
    }
}

void ConsoleManager::switchConsole(std::shared_ptr<AConsole> console) {
    if (console) {
        previousConsole = currentConsole;
        currentConsole = console;
        currentConsole->onEnabled();
        currentConsole->display();

        // Assume ScreenConsole can be cast from AConsole
        if (auto screenConsole = std::dynamic_pointer_cast<ScreenConsole>(console)) {
            activeScreens[screenConsole->getName()] = screenConsole; // Store active screen
        }
    }
}



void ConsoleManager::returnToPreviousConsole() {
    if (previousConsole) {
        std::swap(currentConsole, previousConsole);
        if (auto mainConsole = std::dynamic_pointer_cast<MainConsole>(currentConsole)) {
            mainConsole->display();
        }
        else {
            system("cls");  // Clear the console
            currentConsole->onEnabled();
        }
    }
}


void ConsoleManager::exitApplication() {
    running = false;
}

bool ConsoleManager::isRunning() const {
    return running;
}

HANDLE ConsoleManager::getConsoleHandle() const {
    return consoleHandle;
}

void ConsoleManager::setCursorPosition(int posX, int posY) const {
    COORD coord;
    coord.X = posX;
    coord.Y = posY;
    SetConsoleCursorPosition(consoleHandle, coord);
}

std::shared_ptr<AConsole> ConsoleManager::getCurrentConsole() const {
    return currentConsole;
}

void ConsoleManager::registerScreen(std::shared_ptr<AConsole> screenRef) {
    const std::string& screenName = screenRef->getName(); // Store the name
    if (this->consoleTable.find(screenName) != this->consoleTable.end()) {
        std::cerr << "Screen name " << screenName << " already exists. Please use a different name." << std::endl;
        return;
    }
    this->consoleTable[screenName] = screenRef; // Register the screen with its name
    this->switchConsole(screenName); // Switch to the new screen
}


bool ConsoleManager::screenExists(const std::string& name) const {
    return activeScreens.find(name) != activeScreens.end();
}