#pragma once
#include <memory>
#include "Process.h"

class CPUCore {
private:
    int coreID;
    bool isBusy;
    std::shared_ptr<Process> currentProcess;
    int quantumCycles;

    int quantumUsed; // for rr

public:
    CPUCore(int id, int quantumCycles)
        : coreID(id), quantumCycles(quantumCycles), isBusy(false), quantumUsed(0) {}

    int getCoreID() const { return coreID; }
    bool getIsBusy() const { return isBusy; }
    void setIsBusy(bool status) { isBusy = status; }

    void assignProcess(std::shared_ptr<Process> process);
    void checkAndRunProcess();
    void removeProcessIfDone();

    //for rr
    int getQuantumUsed() const { return quantumUsed; }
    void resetQuantumUsed() { quantumUsed = 0; }
    void incrementQuantumUsed() { quantumUsed++; }
    std::shared_ptr<Process> getCurrentProcess() const { return currentProcess; }
    void clearProcess() {
        currentProcess.reset();
        isBusy = false;
        resetQuantumUsed();
    }
};
