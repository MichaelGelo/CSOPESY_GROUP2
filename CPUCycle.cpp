#include "CPUCycle.h"
#include <iostream>
CPUCycle::CPUCycle() : cycleCount(0), running(false) {}

CPUCycle::~CPUCycle() {
    stopClock();
}

void CPUCycle::startClock(std::string scheduler, int delayPerExec, int numCpu, int batchProcessFreq) {
    if (!running) {
        running = true;
        // Capture this, scheduler, and delayPerExec
        clockThread = std::thread([this, scheduler, delayPerExec, numCpu, batchProcessFreq]() {
            while (running) {
                std::lock_guard<std::mutex> lock(mtx);  // Locking for thread safety

                if (scheduler == "fcfs") {
                    // Implement your FCFS here
                    fcfs(scheduler, delayPerExec, numCpu, batchProcessFreq);
                }
                else if (scheduler == "rr") {
                    // Implement your RR here
                    rr(scheduler, delayPerExec, numCpu, batchProcessFreq);
                }
                else {
                std::cout << "error do something about this";
                }

                cycleCount++; // Increment cycle count
                std::this_thread::sleep_for(std::chrono::milliseconds(delayPerExec)); // Delay using delayPerExec
            }
            });

        clockThread.detach(); // Detach thread (optional but might need reconsideration)
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