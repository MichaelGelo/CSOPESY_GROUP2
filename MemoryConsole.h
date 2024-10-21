#pragma once

#include "AConsole.h"

class MemoryConsole : public AConsole
{
public:
    MemoryConsole();
    void onEnabled() override;
    void display() override;
    void process() override;
};