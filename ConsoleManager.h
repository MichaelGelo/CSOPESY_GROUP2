#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include <Windows.h>
#include "AConsole.h"
#include "ScreenConsole.h"

const std::string MAIN_CONSOLE = "MAIN_CONSOLE";
const std::string MARQUEE_CONSOLE = "MARQUEE_CONSOLE";
const std::string SCHEDULING_CONSOLE = "SCHEDULING_CONSOLE";
const std::string MEMORY_CONSOLE = "MEMORY_CONSOLE";

class ConsoleManager {
public:
    using ConsoleTable = std::unordered_map<std::string, std::shared_ptr<AConsole>>;
    static ConsoleManager* getInstance();
    static void initialize();
    static void destroy();
    void drawConsole() const;
    void process() const;
    void switchConsole(const std::string& consoleName);
    void switchConsole(std::shared_ptr<AConsole> console);
    void returnToPreviousConsole();
    void exitApplication();
    bool isRunning() const;
    HANDLE getConsoleHandle() const;
    void setCursorPosition(int posX, int posY) const;

    // New getter for currentConsole
    std::shared_ptr<AConsole> getCurrentConsole() const;

    std::unordered_map<std::string, std::shared_ptr<ScreenConsole>> activeScreens;
    void registerScreen(std::shared_ptr<AConsole> screenRef);

    bool screenExists(const std::string& name) const;
private:
    ConsoleManager();
    ~ConsoleManager() = default;
    ConsoleManager(ConsoleManager const&) = delete;
    ConsoleManager& operator=(ConsoleManager const&) = delete;

    static ConsoleManager* sharedInstance;
    ConsoleTable consoleTable;
    std::shared_ptr<AConsole> currentConsole;
    std::shared_ptr<AConsole> previousConsole;
    HANDLE consoleHandle;
    bool running = true;
};