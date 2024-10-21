#pragma once
#include <string>

class AConsole
{
public:
    AConsole(std::string name);
    virtual ~AConsole() = default;
    std::string getName() const;
    virtual void onEnabled() = 0;
    virtual void display() = 0;
    virtual void process() = 0;

protected:
    std::string name;

    friend class ConsoleManager;
};