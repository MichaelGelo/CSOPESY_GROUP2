#pragma once

#include "AConsole.h"

class MarqueeConsole : public AConsole
{
public:
    MarqueeConsole();
    void onEnabled() override;
    void display() override;
    void process() override;
};