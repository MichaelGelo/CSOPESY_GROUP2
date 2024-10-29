#pragma once
#include <memory>
#include "Process.h"

class CPUCore {
private:
    int coreID;
    bool isBusy;
    std::unique_ptr<Process> currentProcess;

public:
    CPUCore(int id) : coreID(id), isBusy(false) {}

    int getCoreID() const { return coreID; }
    bool getIsBusy() const { return isBusy; }
    void setIsBusy(bool status) { isBusy = status; }

    void assignProcess(std::unique_ptr<Process> process);
    void checkAndRunProcess();
    void removeProcessIfDone();
};
