#include "CPUCycle.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

CPUCycle::CPUCycle() : cycleCount(0), running(false) {}

CPUCycle::~CPUCycle() {
    stopClock();
}
// Starts the clock
void CPUCycle::startClock() {
    running = true;
    clockThread = std::thread(&CPUCycle::runCycles, this);
    clockThread.detach();
}

void CPUCycle::stopClock() {
    running = false;
    if (clockThread.joinable()) {
        clockThread.join();
    }
}

int CPUCycle::getCurrentCycle() const {
    return cycleCount;
}

void CPUCycle::setCycleDelay(int delayMicroseconds) {
    std::lock_guard<std::mutex> lock(mtx); 
    cycleDelay = delayMicroseconds;
}

// does the cycle things then notifies waiting threads
// added stuff para synchronized sila ni scheduler
void CPUCycle::runCycles() {
    while (running) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            cycleCount++;
            cv.notify_all(); // Notify Scheduler each cycle increment
        }

        // Sleep for the specified delay in microseconds
        std::this_thread::sleep_for(std::chrono::microseconds(cycleDelay));
    }
    std::cout << "runCycles stopped." << std::endl;
}

// just added this ^ if it goes weirdo again