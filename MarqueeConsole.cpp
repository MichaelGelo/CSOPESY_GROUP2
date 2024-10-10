#include <vector> 
#include "MarqueeConsole.h"

MarqueeConsole::MarqueeConsole()
    : logo{ rand() % (WIDTH - 6) + 1, rand() % (HEIGHT - 2) + 1, 1, 1, "Hello World in Marquee!" } {
    srand(static_cast<unsigned>(time(0)));
}

// Clear screen
void MarqueeConsole::clearScreen() {
    system("cls");
}

// Set cursor position
void MarqueeConsole::gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Draw the frame
void MarqueeConsole::drawFrame() {
    clearScreen();

    std::cout << "*************************************************\n";
    std::cout << "* Displaying a marquee console! *\n";
    std::cout << "*************************************************\n";

    gotoxy(logo.x, logo.y + 4); // Position for logo
    std::cout << logo.text;

    // Display command messages
    int messageY = HEIGHT + 6;
    for (const auto& message : commandMessages) {
        gotoxy(0, messageY++);
        std::cout << message;
    }

    gotoxy(0, HEIGHT + 5); // Position for user input
    std::cout << "Enter a command for MARQUEE_CONSOLE:" << userInput;

    std::cout.flush();
}


// Poll for keyboard input
void MarqueeConsole::pollKeyboard() {
    while (_kbhit()) {
        char key = _getch();

        if (key == '\r') { // Enter key
            if (userInput == "exit") {
                running = false;
            }
            else {
                commandMessages.push_back("Command processed in MARQUEE_CONSOLE:" + userInput);
            }
            userInput.clear();
        }
        else if (key == 8) { // Backspace key
            if (!userInput.empty()) userInput.pop_back();
        }
        else {
            userInput += key;
        }

        if (userInput == "exit") {
            std::cout << "\nEnter a command for MARQUEE_CONSOLE:";
        }

        drawFrame();
    }
}

// Update the position of the logo
void MarqueeConsole::updateLogoPosition() {
    logo.x += logo.dx;
    logo.y += logo.dy;

    // Bounce logic
    if (logo.x <= 1) {
        logo.dx = 1;   // Bounce right 
    }
    else if (logo.x >= WIDTH - 4) {
        logo.dx = -1;   // Bounce left 
    }

    if (logo.y <= 1) {
        logo.dy = 1;   // Bounce down 
    }
    else if (logo.y >= HEIGHT - 2) {
        logo.dy = -1;   // Bounce up
    }
}

// Run the main loop
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
    std::cout << "Application closed." << std::endl;
}