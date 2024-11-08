#include "CPUCore.h"
#include "Scheduler.h"
#include "AttachedProcess.h"
#include <iostream>
#include <chrono>

void CPUCore::assignProcess(std::shared_ptr<Process> process) {
    auto attachedProcess = std::dynamic_pointer_cast<AttachedProcess>(process);
    if (!attachedProcess) {
        if (scheduler->attachProcessToMemory(process)) {
            attachedProcess = std::dynamic_pointer_cast<AttachedProcess>(process);
        }
        else {
            std::cout << "Not enough memory to attach process " << process->getPid() << " to memory." << std::endl;
            return;
        }
    }

    // Assign attached process to this core for execution
    currentProcess = std::static_pointer_cast<Process>(attachedProcess);
    isBusy = true;
    quantumUsed = 0;
}



void CPUCore::checkAndRunProcess() {
    if (currentProcess) {
        currentProcess->executeCommand();
        currentProcess->getNextCommand([]() {});
        incrementQuantumUsed(); //added last night
        removeProcessIfDone();
        // std::cout << "The core has commanded."; // for testing only
    }
    else {
        //std::cout << "Core " << coreID << " has no process assigned." << std::endl;
    }
}

void CPUCore::removeProcessIfDone() {
    if (currentProcess) {
        // Check if the process has finished or if its quantum is expired in a round-robin scheduler
        if (currentProcess->hasFinished() ||
            (scheduler->isRoundRobin() && isQuantumExpired())) {

            if (!currentProcess->hasFinished() && scheduler->isRoundRobin()) {
                currentProcess->switchState(Process::WAITING);
                scheduler->addToRQ(currentProcess);  // Re-add process to the Ready Queue if it hasn't finished
            }

            auto attachedProcess = std::dynamic_pointer_cast<AttachedProcess>(currentProcess);
            if (attachedProcess) {
                memoryAllocator->deallocate(attachedProcess);
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
            //std::this_thread::sleep_for(std::chrono::microseconds(1)); // Wait if core is idle
            continue;
        }

        // Wait until delayPerExec cycles have passed in CPUCycle
        int targetCycle = scheduler->getCpuCycle().getCurrentCycle() + delayPerExec + 1;

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
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    }
}

void CPUCore::stopCoreLoop() {
    running = false; 
    scheduler->getCpuCycle().getConditionVariable().notify_all(); 
}
