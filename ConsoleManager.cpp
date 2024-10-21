#include "ConsoleManager.h"
#include "MainConsole.h"
#include "MarqueeConsole.h"
#include "SchedulingConsole.h"
#include "MemoryConsole.h"

ConsoleManager::ConsoleManager()
{
    this->running = true;
    //init consoles
    this->consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    const std::shared_ptr<MainConsole> mainConsole = std::make_shared<MainConsole>();
    const std::shared_ptr<MarqueeConsole> marqueeConsole = std::make_shared<MarqueeConsole>();
    const std::shared_ptr<SchedulingConsole> schedulingConsole = std::make_shared<SchedulingConsole>();
    const std::shared_ptr<MemoryConsole> memoryConsole = std::make_shared<MemoryConsole>();
    this->consoleTable[MAIN_CONSOLE] = mainConsole;
    this->consoleTable[MARQUEE_CONSOLE] = marqueeConsole;
    this->consoleTable[SCHEDULING_CONSOLE] = schedulingConsole;
    this->consoleTable[MEMORY_CONSOLE] = memoryConsole;
    this->switchConsole(MAIN_CONSOLE);
}


ConsoleManager* ConsoleManager::sharedInstance = nullptr;

ConsoleManager* ConsoleManager::getInstance()
{
    if (sharedInstance == nullptr)
    {
        sharedInstance = new ConsoleManager();
    }
    return sharedInstance;
}

void ConsoleManager::initialize()
{
    getInstance();
}

void ConsoleManager::destroy()
{
    delete sharedInstance;
    sharedInstance = nullptr;
}

void ConsoleManager::drawConsole() const
{
    if (currentConsole)
    {
        currentConsole->display();
    }
}

void ConsoleManager::process() const
{
    if (currentConsole)
    {
        currentConsole->process();
    }
}

void ConsoleManager::switchConsole(std::string consoleName)
{
    auto it = consoleTable.find(consoleName);
    if (it != consoleTable.end())
    {
        previousConsole = currentConsole;
        currentConsole = it->second;
        currentConsole->onEnabled();
    }
}

void ConsoleManager::returnToPreviousConsole()
{
    if (previousConsole)
    {
        std::swap(currentConsole, previousConsole);
        currentConsole->onEnabled();
    }
}

void ConsoleManager::exitApplication()
{
    running = false;
}

bool ConsoleManager::isRunning() const
{
    return running;
}

HANDLE ConsoleManager::getConsoleHandle() const
{
    return consoleHandle;
}

void ConsoleManager::setCursorPosition(int posX, int posY) const
{
    COORD coord;
    coord.X = posX;
    coord.Y = posY;
    SetConsoleCursorPosition(consoleHandle, coord);
}