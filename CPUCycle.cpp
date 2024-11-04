#include "CPUCycle.h"
#include <iostream>
#include <chrono>

CPUCycle::CPUCycle() : cycleCount(0), running(false), cycleDelay(5) {}

CPUCycle::~CPUCycle() {
    stopClock();
}

// Starts the clock
void CPUCycle::startClock() {
    running = true;
    clockThread = std::thread(&CPUCycle::runCycles, this);
}

void CPUCycle::stopClock() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        running = false; 
    }
    cv.notify_all();  

    if (clockThread.joinable()) {
        clockThread.join();
    }
}


int CPUCycle::getCurrentCycle() const {
    return cycleCount.load();  // Atomic load
}

void CPUCycle::setCycleDelay(int delayMicroseconds) {
    std::lock_guard<std::mutex> lock(mtx);
    cycleDelay = delayMicroseconds;
}

int CPUCycle::getCycleDelay() const {
    std::lock_guard<std::mutex> lock(mtx);
    return cycleDelay;
}

void CPUCycle::runCycles() {
    while (running) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            cycleCount++;
            cv.notify_all(); // Notify all waiting threads of cycle increment
            // std::cout << "1 cycle" << std::endl; for testing
        }

        // Sleep for the specified delay in microseconds
        std::this_thread::sleep_for(std::chrono::microseconds(cycleDelay));
    }
}
