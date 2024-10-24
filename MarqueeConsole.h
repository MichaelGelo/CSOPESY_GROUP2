#pragma once
#ifndef MARQUEECONSOLE_H
#define MARQUEECONSOLE_H

#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <windows.h>
#include <atomic>
#include <vector>
#include "AConsole.h" 

class MarqueeConsole : public AConsole {
public:
    MarqueeConsole();  
    void run();

    void onEnabled() override;
    void display() override;
    void process() override;

private:
    void clearScreen();
    void gotoxy(int x, int y);
    void drawFrame();
    void pollKeyboard();
    void updateLogoPosition();

    struct Logo {
        int x, y;      
        int dx, dy;     
        const char* text; 
    };

    const int FPS = 30;             // Frames per second
    const int POLLING_RATE = 40;    // Polling rate in milliseconds
    const int WIDTH = 60;          // Console width
    const int HEIGHT = 15;          // Console height

    Logo logo;
    std::string userInput;
    std::vector<std::string> commandMessages;
    std::atomic<bool> running{ true };
};

#endif // MARQUEECONSOLE_H
