#include "AConsole.h"

AConsole::AConsole(std::string name) : name(std::move(name)) {}

std::string AConsole::getName() {
    return name;
}
