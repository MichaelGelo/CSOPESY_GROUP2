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
// added stuff para synchronized sila ni scheduler
void CPUCycle::runCycles() {
    while (running) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            cycleCount++;
            // std::cout << "CPUCycle Count: " << cycleCount << std::endl; 
            cv.notify_all();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "runCycles stopped." << std::endl;
}

// just added this ^ if it goes weirdo again