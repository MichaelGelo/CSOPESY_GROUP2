#include <string>
#include <vector>
#include <iostream>
#include <thread>
#include "Process.h"
#include "Scheduler.h"
#include "CPUCore.h"

// Display scheduler configuration
void Scheduler::displayConfiguration() {
    std::cout << "Scheduler Configuration:" << std::endl;
    std::cout << "Number of CPUs: " << numCpu << std::endl;
    std::cout << "Scheduler Algorithm: " << schedulerAlgorithm << std::endl;
    std::cout << "Quantum Cycles: " << quantumCycles << std::endl;
    std::cout << "Batch Process Frequency: " << batchProcessFreq << std::endl;
    std::cout << "Min Instructions: " << minInstructions << std::endl;
    std::cout << "Max Instructions: " << maxInstructions << std::endl;
    std::cout << "Delay per Execution: " << delayPerExec << std::endl;
}

// Remove quotes from a string
void Scheduler::removeQuotes(std::string& str) {
    if (!str.empty() && str.front() == '"' && str.back() == '"') {
        str = str.substr(1, str.size() - 2);
    }
}

// Initialize cores and start threads
void Scheduler::initializeCores() {
    std::cout << "Scheduler now initializing cores and starting core threads... " << std::endl;

    for (int i = 0; i < numCpu; ++i) {
        // Create a CPU core and pass the Scheduler for access to shared resources
        cores.push_back(std::make_unique<CPUCore>(i, quantumCycles, delayPerExec, this));

        // Start a dedicated thread for each core's loop
        coreThreads.emplace_back(&CPUCore::runCoreLoop, cores.back().get());
    }
}

// Stop all core threads safely
void Scheduler::stopAllCores() {
    for (auto& core : cores) {
        core->stopCoreLoop();
    }

    for (auto& thread : coreThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

// Destructor to clean up threads
Scheduler::~Scheduler() {
    schedulerStop();  

    for (auto& thread : coreThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    cpuCycle.stopClock();  
}

// Add process to the ready queue
void Scheduler::addToRQ(std::shared_ptr<Process> process) {
    std::lock_guard<std::mutex> lock(rqMutex);
    if (schedulerAlgorithm == "fcfs") {
        process->switchState(Process::RUNNING);
    }
    if (schedulerAlgorithm == "rr") {
        process->switchState(Process::READY);
    }
    rq.push(process);
    rqCondition.notify_all();
}

// Implement FCFS scheduling
void Scheduler::fcfs() {
    std::cout << "Scheduler started with First-Come, First-Served (FCFS) algorithm." << std::endl;
    schedulerStatus = true;
    std::thread(&Scheduler::listenForCycle, this).detach();
}

// Listen for cycle updates and assign processes to cores
void Scheduler::listenForCycle() {
    while (schedulerStatus) {
        // Wait for CPU cycles to be available and check for free cores
        std::unique_lock<std::mutex> lock(cpuCycle.getMutex());
        cpuCycle.getConditionVariable().wait(lock, [this] {
            return !schedulerStatus || !rq.empty();
            });

        if (!schedulerStatus) break;

        for (auto& core : cores) {
            // Check if the core is not busy
            if (!core->getIsBusy()) {
                std::unique_lock<std::mutex> rqLock(rqMutex);
                if (!rq.empty()) {
                    auto process = rq.front();
                    rq.pop();
                    rqLock.unlock();

                    core->assignProcess(process);
                    process->setCore(core->getCoreID());
                    process->switchState(Process::RUNNING);
                    core->setIsBusy(true);
                }
            }
        }
    }
}

// Stop the scheduler
void Scheduler::schedulerStop() {
    schedulerStatus = false;
    cv.notify_all();

    for (auto& core : cores) {
        core->stopCoreLoop();
    }
    cpuCycle.getConditionVariable().notify_all();
}


// Implement Round Robin (RR) scheduling
/*void Scheduler::rr() {
    std::cout << "Scheduler started with Round Robin (RR) algorithm." << std::endl;
    schedulerStatus = true;

    std::thread([this]() {
        while (schedulerStatus) {
            std::unique_lock<std::mutex> lock(cpuCycle.getMutex());
            cpuCycle.getConditionVariable().wait(lock, [this] {
                return !schedulerStatus || cpuCycle.isRunning() || !rq.empty();
                });

            if (!schedulerStatus) break;

            for (auto& core : cores) {
                // Handle the currently running process
                if (core->getIsBusy()) {
                    core->incrementQuantumUsed();
                    core->checkAndRunProcess();

                    if (delayPerExec > 0) {
                        std::this_thread::sleep_for(std::chrono::microseconds(delayPerExec));
                    }

                    // Check if quantum is exhausted or process has finished
                    if (core->getQuantumUsed() >= quantumCycles ||
                        (core->getCurrentProcess() && core->getCurrentProcess()->hasFinished())) {

                        auto currentProcess = core->getCurrentProcess();
                        if (currentProcess && !currentProcess->hasFinished()) {
                            std::unique_lock<std::mutex> rqLock(rqMutex);
                            rq.push(currentProcess);
                        }
                        core->clearProcess();
                    }
                }

                // If core is free, assign a new process from the ready queue
                if (!core->getIsBusy()) {
                    std::unique_lock<std::mutex> rqLock(rqMutex);
                    if (!rq.empty()) {
                        auto process = rq.front();
                        rq.pop();
                        rqLock.unlock();

                        core->assignProcess(process);
                        core->resetQuantumUsed();
                        process->switchState(Process::RUNNING);
                    }
                }
            }
        }
        }).detach();
}
*/

void Scheduler::rr() {
    std::cout << "Scheduler started with Round Robin algorithm." << std::endl;
    schedulerStatus = true;

    // Start the cycle listener thread specifically for Round Robin
    std::thread(&Scheduler::listenForCycleRR, this).detach();
}

void Scheduler::listenForCycleRR() {
    while (schedulerStatus) {
        std::unique_lock<std::mutex> lock(rqMutex);

        // Wait until there are processes or scheduler is stopped
        rqCondition.wait(lock, [this] {
            return !schedulerStatus || !rq.empty();
            });

        if (!schedulerStatus) break;

        // Check for available cores
        for (auto& core : cores) {
            if (!core->getIsBusy() && !rq.empty()) {
                auto process = rq.front();
                rq.pop();
                lock.unlock();

                // Assign process to core
                core->assignProcess(process);
                process->setCore(core->getCoreID());
                process->switchState(Process::RUNNING);
                core->setIsBusy(true);

                // Requeue the process if it hasn't completed
                /*std::thread([this, process, &core]() {
                    // Wait for the process to complete its time quantum
                    std::this_thread::sleep_for(std::chrono::milliseconds(quantumCycles));

                    // If process is not complete, move back to queue
                    if (process->getRemainingInstructions() > 0) {
                        process->switchState(Process::WAITING);
                        std::lock_guard<std::mutex> requeueLock(rqMutex);
                        rq.push(process);
                        core->setIsBusy(false);
                        rqCondition.notify_all();
                    }
                */
                break; // Assign to one core at a time
            }
        }
    }
}


