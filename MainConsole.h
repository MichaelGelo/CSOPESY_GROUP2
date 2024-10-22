#ifndef MAIN_CONSOLE_H
#define MAIN_CONSOLE_H

#include "AConsole.h"
#include <vector>
#include <string>
#include <memory>
#include "ScreenConsole.h"
#include <functional>
#include "Process.h"
class MainConsole : public AConsole {
public:
    MainConsole();
    void onEnabled() override;
    void display() override;
    void process() override;
    void menu() const;
    void enter() const;
    void color(int n) const;

    std::vector<std::shared_ptr<Process>> processes;
    int nextPid = 1;
    void displayProcessStatus() const;

private:
    void captureAndStoreOutput(std::function<void()> func);
    std::vector<std::string> commandHist;

    void showHistory() const;
    bool menuShown;
    void clear();
};

#endif