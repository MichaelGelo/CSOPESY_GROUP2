#pragma once
// MarqueeConsole.h
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

class MarqueeConsole {
public:
    MarqueeConsole();
    void run();

private:
    void clearScreen();
    void gotoxy(int x, int y);
    void drawFrame();
    void pollKeyboard();
    void updateLogoPosition();

    struct Logo {
        int x, y;     // Pos
        int dx, dy;   // Direction
        const char* text; // text
    };

    // Params
    const int FPS = 30;            // FPS
    const int POLLING_RATE = 40;    // Polling rate (ms)

    const int WIDTH = 100;          // Console Width
    const int HEIGHT = 15;         // Console Height

    Logo logo;
    std::string userInput;
    std::vector<std::string> commandMessages;
    std::atomic<bool> running{ true };
};

#endif // MARQUEECONSOLE_H