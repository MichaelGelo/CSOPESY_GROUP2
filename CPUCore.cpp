#include "CPUCore.h"
#include "Scheduler.h"
#include "AttachedProcess.h"
#include <iostream>
#include <chrono>

void CPUCore::assignProcess(std::shared_ptr<AttachedProcess> process) {
    if (!scheduler->attachProcessToMemory(process)) {
        std::cout << "Not enough memory to attach process " << process->getPid() << " to memory." << std::endl;
        return;
    }
    else {
        currentProcess = process;
        isBusy = true;
        quantumUsed = 0;
    }
}

void CPUCore::checkAndRunProcess() {
    if (currentProcess) {
        currentProcess->executeCommand();
        currentProcess->getNextCommand([]() {});
        incrementQuantumUsed();
        removeProcessIfDone();
    }
}

void CPUCore::removeProcessIfDone() {
    if (currentProcess) {
        bool processNeedsDeallocation = currentProcess->hasFinished() || 
            (scheduler->isRoundRobin() && isQuantumExpired());

        if (processNeedsDeallocation) {
            if (!currentProcess->hasFinished() && scheduler->isRoundRobin()) {
                currentProcess->switchState(Process::READY);
                scheduler->addToRQ(currentProcess);
            } else {
                currentProcess->switchState(Process::FINISHED);
            }

            if (currentProcess->hasFinished())
            memoryAllocator->deallocate(currentProcess);

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
        if (!isBusy) continue;

        // Wait until delayPerExec cycles have passed
        int targetCycle = scheduler->getCpuCycle().getCurrentCycle() + delayPerExec + 1;

        {
            std::unique_lock<std::mutex> lock(cycleMutex);
            cycleCondition.wait(lock, [&]() {
                return scheduler->getCpuCycle().getCurrentCycle() >= targetCycle || !running;
                });
        }

        if (!running) break;

        checkAndRunProcess();

        if (scheduler->isRoundRobin() && currentProcess && isQuantumExpired()) {
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
