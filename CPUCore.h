#pragma once
#include <memory>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <mutex>
#include "Process.h"

class Scheduler; // to access rq 

class CPUCore {
private:
    int coreID;
    bool isBusy;
    std::shared_ptr<Process> currentProcess;
    int quantumCycles;
    int quantumUsed;
    int delayPerExec;
    std::atomic<bool> running;
    Scheduler* scheduler; 

public:
    CPUCore(int id, int quantumCycles, int delayPerExec, Scheduler* scheduler)
        : coreID(id), quantumCycles(quantumCycles), delayPerExec(delayPerExec), quantumUsed(0),
        isBusy(false), running(true), scheduler(scheduler) {}

    int getCoreID() const { return coreID; }
    bool getIsBusy() const { return isBusy; }
    void setIsBusy(bool status) { isBusy = status; }

    void assignProcess(std::shared_ptr<Process> process);
    void checkAndRunProcess();
    void removeProcessIfDone();

    //
    bool isQuantumExpired() const {
        return quantumUsed >= quantumCycles;
    }


    // rr methods
    void resetQuantumUsed() { quantumUsed = 0; }
    void incrementQuantumUsed() { quantumUsed++; }
    int getQuantumUsed() const { return quantumUsed; }

    std::shared_ptr<Process> getCurrentProcess() const { return currentProcess; }
    void clearProcess();

    void waitForCycleAndExecute(std::condition_variable& cycleCondition, std::mutex& cycleMutex, int delayPerExec);
    void runCoreLoop();
    void stopCoreLoop();
};
