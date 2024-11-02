#include "CPUCore.h"
#include "Scheduler.h"
#include <iostream>
#include <chrono>

void CPUCore::assignProcess(std::shared_ptr<Process> process) {
    currentProcess = process;
    isBusy = true;
    quantumUsed = 0; // Reset quantum usage when a new process is assigned
}

void CPUCore::checkAndRunProcess() {
    if (currentProcess) {
        currentProcess->executeCommand();
        currentProcess->getNextCommand([]() {});
        incrementQuantumUsed(); //added last night
        removeProcessIfDone();
    }
    else {
        std::cout << "Core " << coreID << " has no process assigned." << std::endl;
    }
}

void CPUCore::removeProcessIfDone() {
    /*if (currentProcess && (currentProcess->hasFinished() ||
        (scheduler->isRoundRobin() && quantumUsed >= quantumCycles))) {

        if (!currentProcess->hasFinished() && scheduler->isRoundRobin()) {
            scheduler->addToRQ(currentProcess);
        }

        clearProcess();  // Clear the core for the next process
    }*/
    if (currentProcess) {
        if (currentProcess->hasFinished() ||
            (scheduler->isRoundRobin() && isQuantumExpired())) {

            if (!currentProcess->hasFinished() && scheduler->isRoundRobin()) {
                currentProcess->switchState(Process::WAITING);
                scheduler->addToRQ(currentProcess);
            }
            clearProcess();
        }
    }
}


void CPUCore::clearProcess() {
    currentProcess.reset();
    isBusy = false;
    resetQuantumUsed();
}

void CPUCore::waitForCycleAndExecute(std::condition_variable& cycleCondition, std::mutex& cycleMutex, int delayPerExec) {
    while (running) {
        if (!isBusy) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Wait if core is idle
            continue;
        }

        // Wait until delayPerExec cycles have passed in CPUCycle
        int targetCycle = scheduler->getCpuCycle().getCurrentCycle() + delayPerExec;

        {
            std::unique_lock<std::mutex> lock(cycleMutex);
            cycleCondition.wait(lock, [&]() {
                return scheduler->getCpuCycle().getCurrentCycle() >= targetCycle || !running;
                });
        }

        if (!running) break; 

        checkAndRunProcess();

        if (scheduler->isRoundRobin()) {
            //incrementQuantumUsed();
            if (currentProcess && isQuantumExpired()) {

                if (!currentProcess->hasFinished()) {
                    currentProcess->switchState(Process::WAITING);
                    scheduler->addToRQ(currentProcess);
                }
                clearProcess();
            }
        }

        if (currentProcess && (currentProcess->hasFinished() ||
            (scheduler->isRoundRobin() && quantumUsed >= quantumCycles))) {
            currentProcess->switchState(Process::WAITING);
            scheduler->addToRQ(currentProcess); 
            clearProcess();
        }
    }
}


void CPUCore::runCoreLoop() {
    while (running) {
        if (isBusy && currentProcess) {
            waitForCycleAndExecute(
                scheduler->getCpuCycle().getConditionVariable(),
                scheduler->getCpuCycle().getMutex(),
                delayPerExec
            );
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void CPUCore::stopCoreLoop() {
    running = false; 
    scheduler->getCpuCycle().getConditionVariable().notify_all(); 
}
