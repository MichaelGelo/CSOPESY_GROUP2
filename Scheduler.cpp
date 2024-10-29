#include <string>
#include <vector>
#include <iostream>
#include "Process.h"
#include "Scheduler.h"
#include "CPUCore.h"

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

void Scheduler::removeQuotes(std::string& str) {
    if (!str.empty() && str.front() == '"' && str.back() == '"') {
        str = str.substr(1, str.size() - 2);  
    }
}

void Scheduler::initializeCores() {

    std::cout << "Scheduler now initializing cores... " << std::endl;

    for (int i = 0; i < numCpu; ++i) {
        cores.emplace_back(i);  
    }
}

void Scheduler::addToRQ(std::shared_ptr<Process> process) {
    std::lock_guard<std::mutex> lock(rqMutex);
    rq.push(process);
    rqCondition.notify_all();
    std::cout << "Process added to ready queue with PID: " << process->getPid() << std::endl;
}

void Scheduler::fcfs() {
    std::cout << "Scheduler started with First-Come, First-Served (FCFS) algorithm." << std::endl;
    schedulerStatus = true;
    std::thread(&Scheduler::listenForCycle, this).detach();
}

void Scheduler::listenForCycle() {
    while (schedulerStatus) {
        std::unique_lock<std::mutex> lock(cpuCycle.getMutex());
        cpuCycle.getConditionVariable().wait(lock, [this] {
            return !schedulerStatus || cpuCycle.isRunning() || !rq.empty();
            });

        if (!schedulerStatus) break;

        int currentCycle = cpuCycle.getCurrentCycle();
        // std::cout << "Notification Cycle Count: " << currentCycle << std::endl; // just to check if synchronized


        // Process available cores and assign processes from rq
        for (auto& core : cores) {
            if (!core.getIsBusy()) {
                std::unique_lock<std::mutex> rqLock(rqMutex); 
                if (!rq.empty()) {
                    auto process = rq.front();
                    rq.pop();
                    rqLock.unlock();  

                    core.assignProcess(process); 
                    std::cout << "Assigned process to core " << core.getCoreID() << std::endl;
                }
            }
        }
    }
}

    // TODO
    // nvm i won't combine them para easy debug
    // [x] add processes to ready queue
    // [x] access ready queue
    // [x] synchronized with cpuCycle
    // [x] can determine if its fcfs or rr
    // [x] somewhat implemented stuff sa cpucore 

  // each notification from CPUCycle, check readyQueue
  // check if there's an open cpuCore
  // if yes, put the process in that free cpuCore


void Scheduler::rr() {

}

void Scheduler::schedulerTest() {

}

void Scheduler::schedulerStop() {
    schedulerStatus = false;
    cv.notify_all();
}


//for cpuCore
//check if may laman siya na process
//if notify, run si Process executeCommand() and getNextCommand()
//if current lines == max lines or done na ung process, tanggalin na

// quantum ni kai for round robin
