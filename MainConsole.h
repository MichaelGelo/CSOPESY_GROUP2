#ifndef MAIN_CONSOLE_H
#define MAIN_CONSOLE_H

#include "AConsole.h"
#include <vector>
#include <string>
#include <memory>
#include "ScreenConsole.h"

class MainConsole : public AConsole {
public:
    MainConsole();
    void onEnabled() override;
    void display() override;
    void process() override;
    void menu() const;
    void enter() const;
    void clear() const;
    void color(int n) const;

private:
    std::vector<std::string> commandHist;
    void showHistory() const;
    bool menuShown;
};

#endif