#pragma once

#include "AConsole.h"

class SchedulingConsole : public AConsole
{
public:
    SchedulingConsole();
    void onEnabled() override;
    void display() override;
    void process() override;
};