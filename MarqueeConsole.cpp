#include <vector>
#include "MarqueeConsole.h"
#include "AConsole.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <conio.h>
#include <windows.h>

MarqueeConsole::MarqueeConsole()
    : AConsole("MARQUEE_CONSOLE"), 
    logo{ rand() % (WIDTH - 6) + 1, rand() % (HEIGHT - 2) + 1, 1, 1, "Hello World in Marquee!" } {
    srand(static_cast<unsigned>(time(0)));
}

void MarqueeConsole::onEnabled() {
    std::cout << "Marquee Console Enabled!" << std::endl;
}

void MarqueeConsole::clearScreen() {
    system("cls");
}

void MarqueeConsole::gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void MarqueeConsole::drawFrame() {
    clearScreen();

    std::cout << "*************************************************\n";
    std::cout << "* Displaying a marquee console! *\n";
    std::cout << "*************************************************\n";

    gotoxy(logo.x, logo.y + 4);
    std::cout << logo.text;

    int messageY = HEIGHT + 6;
    for (const auto& message : commandMessages) {
        gotoxy(0, messageY++);
        std::cout << message;
    }

    gotoxy(0, HEIGHT + 5); 
    std::cout << "Enter a command for MARQUEE_CONSOLE: " << userInput;

    std::cout.flush();
}

void MarqueeConsole::pollKeyboard() {
    while (_kbhit()) {
        char key = _getch();

        if (key == '\r') { 
            if (userInput == "exit") {
                running = false;
            }
            else {
                commandMessages.push_back("Command processed in MARQUEE_CONSOLE: " + userInput);
            }
            userInput.clear();
        }
        else if (key == 8) { // Backspace key
            if (!userInput.empty()) userInput.pop_back();
        }
        else {
            userInput += key;
        }

        drawFrame();
    }
}

void MarqueeConsole::updateLogoPosition() {
    logo.x += logo.dx;
    logo.y += logo.dy;

    if (logo.x <= 1) {
        logo.dx = 1; 
    }
    else if (logo.x >= WIDTH - 4) {
        logo.dx = -1;  
    }

    if (logo.y <= 1) {
        logo.dy = 1;  
    }
    else if (logo.y >= HEIGHT - 2) {
        logo.dy = -1;  
    }
}

void MarqueeConsole::run() {
    auto lastFrameTime = std::chrono::steady_clock::now();

    while (running) {
        auto currentTime = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastFrameTime).count() >= (1000 / FPS)) {
            updateLogoPosition();
            drawFrame();
            lastFrameTime = currentTime;
        }

        pollKeyboard();
        std::this_thread::sleep_for(std::chrono::milliseconds(POLLING_RATE)); // polling rate
    }

    clearScreen();
    std::cout << "MarqueeConsole closed." << std::endl;
}

void MarqueeConsole::display() {
    std::cout << "Starting Marquee Console..." << std::endl;
    run();
}

void MarqueeConsole::process() {
    std::thread keyboardThread(&MarqueeConsole::pollKeyboard, this);
    keyboardThread.detach(); 

    std::cout << "Marquee animation is running. Press 'exit' to stop." << std::endl;
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(POLLING_RATE));
    }

    std::cout << "Returning to Main Console..." << std::endl;
}
