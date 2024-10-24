#include "CPUCycle.h"

CPUCycle::CPUCycle() : cycleCount(0), running(false) {}

CPUCycle::~CPUCycle() {
    stopClock();
}

void CPUCycle::startClock() {
    if (!running) {
        running = true;
        clockThread = std::thread([this]() {
            while (running) {
                std::lock_guard<std::mutex> lock(mtx);
                cycleCount++;
                std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Adjust nlang tick rate
            }
            });
        clockThread.detach();
    }
}

void CPUCycle::stopClock() {
    running = false;
}

int CPUCycle::getCurrentCycle() const {
    return cycleCount;
}

bool CPUCycle::isRunning() const {
    return running;
}