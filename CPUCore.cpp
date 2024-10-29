#include "CPUCore.h"
#include <iostream>

void CPUCore::assignProcess(std::unique_ptr<Process> process) {
    currentProcess = std::move(process);
    isBusy = true;
}

void CPUCore::checkAndRunProcess() {
    if (currentProcess) {  // check if there's a process assigned to this core
        std::cout << "Core " << coreID << " running process " << currentProcess->getPid() << std::endl;

        // Execute the current command and get the next one
        currentProcess->executeCommand([]() {
            // sample line, delete
            std::cout << "Executing command..." << std::endl;
            });
        currentProcess->getNextCommand([]() {
            // sample line, delete
            std::cout << "Getting next command..." << std::endl;
            });

        removeProcessIfDone();
    }
    else {
        std::cout << "Core " << coreID << " has no process assigned." << std::endl;
    }
}

void CPUCore::removeProcessIfDone() {
    if (currentProcess && (currentProcess->hasFinished() || currentProcess->getCurLines() >= currentProcess->getMaxLines())) {
        std::cout << "Process " << currentProcess->getPid() << " has completed on core " << coreID << "." << std::endl;

        currentProcess.reset();  // remove the process
        isBusy = false;         
    }
}
