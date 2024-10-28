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

// does the cycle things then notifies waiting threads
void CPUCycle::runCycles() {
    while (running) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            cycleCount++;
            std::cout << "CPU Cycle Count: " << cycleCount << std::endl; //testing only, pls delete
            std::this_thread::sleep_for(std::chrono::milliseconds(200)); // ito din
            cv.notify_all(); // Notify all waiting threads
        }
    }
}
