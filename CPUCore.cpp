#include "CPUCore.h"
#include <iostream>

// Helper function to strip double quotes from a string
std::string stripQuotes(const std::string& str) {
    if (str.size() >= 1 && str.front() == '"' && str.back() == '"') {
        return str.substr(1, str.size() - 2);
    }
    return str;
}

// Constructor to initialize the core using the Scheduler instance
CpuCore::CpuCore(int coreNumber, std::shared_ptr<Scheduler> scheduler)
    : coreNumber(coreNumber), schedulerInstance(scheduler) {

    std::string schedulerType = stripQuotes(schedulerInstance->getSchedulerAlgorithm());
    quantumTime = schedulerInstance->getQuantumCycles();

    std::cout << "[DEBUG] Initializing CpuCore #" << coreNumber << "\n";
    std::cout << "[DEBUG] Scheduler type (after stripping quotes): " << schedulerType << "\n";
    std::cout << "[DEBUG] Quantum time set to: " << quantumTime << " milliseconds\n";

    // Determine the scheduling mode based on the schedulerType
    if (schedulerType == "fcfs") {
        mode = SchedulingMode::FCFS;
        std::cout << "[INFO] Scheduling mode set to FCFS for CpuCore #" << coreNumber << "\n";
    }
    else if (schedulerType == "rr") {
        mode = SchedulingMode::ROUND_ROBIN;
        std::cout << "[INFO] Scheduling mode set to Round Robin for CpuCore #" << coreNumber << "\n";
    }
    else {
        std::cerr << "[ERROR] Unknown scheduler type: " << schedulerType << ". Defaulting to FCFS." << std::endl;
        mode = SchedulingMode::FCFS;
    }
}

// Check if there are any processes in the queue
bool CpuCore::hasProcesses() const {
    
}

// Main method to execute based on scheduling mode
void CpuCore::execute() {

}

// Method that executes processes using Round Robin scheduling
void CpuCore::runRoundRobin() {
    using namespace std::chrono;

    std::cout << "[DEBUG] Running Round Robin scheduling on CpuCore #" << coreNumber << "\n";

    while (!processQueue.empty()) {
        auto currentProcess = processQueue.front();
        processQueue.pop();
        std::cout << "[INFO] Starting execution of process \"" << currentProcess->getScreenName() << "\" on CpuCore #" << coreNumber << "\n";

        if (currentProcess->getState() == Process::FINISHED) {
            std::cout << "[DEBUG] Process \"" << currentProcess->getScreenName() << "\" is already finished.\n";
            currentProcess->displayProcessInfo();
            continue;
        }

        currentProcess->switchState(Process::RUNNING);

        // Simulating execution for the given quantum time
        auto start = high_resolution_clock::now();
        auto end = start + milliseconds(quantumTime);

        while (high_resolution_clock::now() < end && !currentProcess->hasFinished()) {
            // Execute the process (could be a placeholder operation)
            currentProcess->executeCommand([] {
                std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulated task
                });
        }

        if (currentProcess->getState() != Process::FINISHED) {
            std::cout << "[INFO] Process \"" << currentProcess->getScreenName() << "\" did not finish. Returning to queue.\n";
            // If process is not finished, change its state to READY and put it back in the queue
            currentProcess->switchState(Process::READY);
            processQueue.push(currentProcess);
        }
        else {
            std::cout << "[INFO] Process \"" << currentProcess->getScreenName() << "\" finished on CpuCore #" << coreNumber << "\n";
            currentProcess->displayProcessInfo();
        }
    }

    std::cout << "[DEBUG] Round Robin scheduling completed on CpuCore #" << coreNumber << "\n";
}

// Placeholder for FCFS method (you need to implement it if required)
void CpuCore::runFCFS() {
    std::cout << "[DEBUG] Running FCFS scheduling on CpuCore #" << coreNumber << "\n";

    while (!processQueue.empty()) {
        auto currentProcess = processQueue.front();
        processQueue.pop();
        std::cout << "[INFO] Starting execution of process \"" << currentProcess->getScreenName() << "\" on CpuCore #" << coreNumber << "\n";

        if (currentProcess->getState() == Process::FINISHED) {
            std::cout << "[DEBUG] Process \"" << currentProcess->getScreenName() << "\" is already finished.\n";
            currentProcess->displayProcessInfo();
            continue;
        }

        currentProcess->switchState(Process::RUNNING);

        // Simulate process execution until it finishes (no preemption in FCFS)
        while (!currentProcess->hasFinished()) {
            currentProcess->executeCommand([] {
                std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulated task
                });
        }

        currentProcess->switchState(Process::FINISHED);
        std::cout << "[INFO] Process \"" << currentProcess->getScreenName() << "\" finished on CpuCore #" << coreNumber << "\n";
        currentProcess->displayProcessInfo();
    }

    std::cout << "[DEBUG] FCFS scheduling completed on CpuCore #" << coreNumber << "\n";
}
