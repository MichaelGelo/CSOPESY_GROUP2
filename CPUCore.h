#pragma once
#include <memory>
#include "Process.h"

class CPUCore {
private:
    int coreID;
    bool isBusy;
    std::shared_ptr<Process> currentProcess;
    int quantumCycles;

public:
    CPUCore(int id, int quantumCycles) : coreID(id), quantumCycles(quantumCycles), isBusy(false) {} //added the quantumCycles to the Core for RR

    int getCoreID() const { return coreID; }
    bool getIsBusy() const { return isBusy; }
    void setIsBusy(bool status) { isBusy = status; }

    void assignProcess(std::shared_ptr<Process> process);
    void checkAndRunProcess();
    void removeProcessIfDone();
};
