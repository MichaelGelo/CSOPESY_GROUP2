#include <string>
#include <vector>
#include <iostream>
#include <thread>
#include "AttachedProcess.h"
#include "Scheduler.h"
#include "FlatMemoryAllocator.h"
#include "CPUCore.h"
#include <fstream>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <memory>
#include <unordered_set> 
#include <filesystem>

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

    // added for mo2
    std::cout << "Maximum Overall Memory: " << maxOverallMem << std::endl;
    std::cout << "Memory per Frame: " << memPerFrame << std::endl;
    std::cout << "Minimum Memory per Process: " << minMemPerProc << std::endl;
    std::cout << "Maximum Memory per Process: " << maxMemPerProc << std::endl;

}

// FOR DEBUGGING PURPOSES ONLY

void logMessage(const std::string& message) {
    std::ofstream logFile("allocation_log.txt", std::ios::app); // Append mode
    if (logFile.is_open()) {
        logFile << message << std::endl;
    }
    else {
        std::cerr << "Failed to open log file." << std::endl;
    }
}

// FOR DEBUGGING PURPOSES ONLY

bool Scheduler::attachProcessToMemory(std::shared_ptr<AttachedProcess>& process) {
    if (process->hasAllocated()) {
        logMessage("Process " + std::to_string(process->getPid()) + " is already attached to memory.");
        return true; // Already attached, no need to attach again
    }

    int requiredMemory = process->getMemoryRequirement();
    logMessage("Attempting to attach process " + std::to_string(process->getPid()) + " with memory requirement: " + std::to_string(requiredMemory) + " bytes");

    if (memoryAllocator->getFreeMemory() >= requiredMemory) {
        try {
            void* memoryLocation = memoryAllocator->allocate(process);
            if (memoryLocation) {
                process->allocateResources();

                process->setMemoryLocation(memoryLocation);

                logMessage("Memory location set for process " +
                    std::to_string(process->getPid()) +
                    " at address: " + std::to_string(reinterpret_cast<uintptr_t>(process->getMemoryLocation())) + "\n");


               // logMessage("Current Memory Layout:\n" + getMemoryPrintout());
                return true;
            }
            else {
                std::cerr << "Error: Memory allocation failed, received nullptr." << std::endl;
                logMessage("Memory allocation failed, received nullptr.");
            }
        }
        catch (const std::bad_alloc&) {
            std::cerr << "Memory allocation failed for process " << process->getPid() << std::endl;
            logMessage("Memory allocation failed for process " + std::to_string(process->getPid()));
        }
    }

    logMessage("Insufficient memory for process " + std::to_string(process->getPid()));
    return false;
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
        cores.push_back(std::make_unique<CPUCore>(i, quantumCycles, delayPerExec, this, memoryAllocator));

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
void Scheduler::addToRQ(std::shared_ptr<AttachedProcess> process) {
    if (!process) {
        std::cerr << "Error: Trying to add a nullptr process to the ready queue." << std::endl;
        return;
    }
    std::lock_guard<std::mutex> lock(rqMutex);
    process->switchState(Process::READY);
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
    int currentQuantumCycle = 1;
    while (schedulerStatus) {
        generateMemoryReport(currentQuantumCycle);
        currentQuantumCycle++;

        std::unique_lock<std::mutex> lock(cpuCycle.getMutex());
        cpuCycle.getConditionVariable().wait(lock, [this] {
            return !schedulerStatus || !rq.empty();
            });
        if (!schedulerStatus) break;

        for (auto& core : cores) {
            if (!core) continue;
            if (!core->getIsBusy()) {
                std::unique_lock<std::mutex> rqLock(rqMutex);
                if (!rq.empty()) {
                    auto attachedProcess = std::dynamic_pointer_cast<AttachedProcess>(rq.front());
                    rq.pop();
                    rqLock.unlock();

                    if (attachedProcess) {
                        core->assignProcess(attachedProcess);
                        attachedProcess->setCore(core->getCoreID());
                        attachedProcess->switchState(Process::RUNNING);
                        core->setIsBusy(true);
                    }
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

void Scheduler::rr() {
    std::cout << "Scheduler started with Round Robin algorithm." << std::endl;
    schedulerStatus = true;

    // Start the cycle listener thread specifically for Round Robin
    std::thread(&Scheduler::listenForCycleRR, this).detach();
}

void Scheduler::listenForCycleRR() {
    int currentQuantumCycle = 1;
    while (schedulerStatus) {
        std::unique_lock<std::mutex> lock(rqMutex);

        generateMemoryReport(currentQuantumCycle);
        currentQuantumCycle++;

        rqCondition.wait(lock, [this] {
            return !schedulerStatus || !rq.empty();
            });

        if (!schedulerStatus) break;

        for (auto& core : cores) {
            if (!core->getIsBusy() && !rq.empty()) {
                auto attachedProcess = std::dynamic_pointer_cast<AttachedProcess>(rq.front());
                rq.pop();

                if (!attachedProcess) {
                    std::cerr << "Error: Process cannot be cast to AttachedProcess." << std::endl;
                    continue;
                }

                lock.unlock();

                core->assignProcess(attachedProcess);
                attachedProcess->setCore(core->getCoreID());
                attachedProcess->switchState(Process::RUNNING);
                core->setIsBusy(true);

                break;
            }
        }
    }
}


void Scheduler::generateMemoryReport(int currentQuantumCycle) {
    //std::cout << "Printing to file memory log...\n"; // for debugging lng, can remove

    static bool firstRun = true;
    std::string directoryPath = "./Memory_Stamps";

    // Delete the contents of the directory if it's the first run
    if (firstRun) {
        try {
            if (std::filesystem::exists(directoryPath)) {
                for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
                    std::filesystem::remove_all(entry.path());
                }
            }
            else {
                std::filesystem::create_directory(directoryPath);
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error deleting directory contents: " << e.what() << std::endl;
        }
        firstRun = false;
    }

    std::ostringstream fileNameStream;
    fileNameStream << directoryPath << "/memory_stamp_" << currentQuantumCycle << ".txt";
    std::string fileName = fileNameStream.str();
    std::ofstream reportFile(fileName, std::ios::out);
    if (!reportFile.is_open()) {
        std::cerr << "Failed to open report file: " << fileName << std::endl;
        return;
    }

    // Timestamp
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    reportFile << "Timestamp: (" << std::put_time(std::localtime(&now_c), "%m/%d/%Y %I:%M:%S%p") << ")\n";

    // Number of processes in memory
    int numProcessesInMemory = calculateProcessesInMemory();
    reportFile << "Number of processes in memory: " << numProcessesInMemory << "\n";

    // Total external fragmentation in KB
    int totalExternalFragmentation = calculateExternalFragmentation();
    reportFile << "Total external fragmentation in KB: " << totalExternalFragmentation << "\n";

    // An ASCII printout of the memory
    reportFile << getMemoryPrintout() << "\n";

    reportFile.close();
}



int Scheduler::calculateProcessesInMemory() {
    int processCount = 0;

    // Loop through the memory partitions and count allocated processes
    auto memoryPartitions = memoryAllocator->getMemoryPartitions();
    for (const auto& partition : memoryPartitions) {
        if (partition.process != nullptr) { // Only count if a process is assigned
            processCount++;
        }
    }

    return processCount;
}


int Scheduler::calculateExternalFragmentation() {
    // Retrieve total memory information
    size_t totalMemory = memoryAllocator->getMaximumSize();
    size_t allocatedMemory = memoryAllocator->getAllocatedSize();
    size_t freeMemory = totalMemory - allocatedMemory; // Total free memory
    size_t minimumAllocatableSize = 4096; // Your defined minimum allocation size
    size_t externalFragmentation = 0; // Initialize external fragmentation counter

    // Loop through the allocation map to find free blocks
    for (size_t i = 0; i < totalMemory; ++i) {
        if (!memoryAllocator->isAllocated(i)) { // If the block is free
            size_t freeBlockSize = 0;

            // Count the size of this contiguous free block
            while (i < totalMemory && !memoryAllocator->isAllocated(i)) {
                freeBlockSize++;
                i++;
            }

            // If the free block is less than the minimum allocatable size, add it to fragmentation
            if (freeBlockSize < minimumAllocatableSize) {
                externalFragmentation += freeBlockSize; // Accumulate the size
            }
        }
    }

    // Return the total external fragmentation
    return externalFragmentation; // Return in bytes, adjust if needed
}


std::string Scheduler::getMemoryPrintout() {
    std::ostringstream output;
    size_t totalMemory = memoryAllocator->getMaximumSize();
    size_t frameSize = memoryAllocator->getMemoryPerFrame(); 
    size_t externalFragmentation = 0;

    output << "----end---- = " << totalMemory << "\n";


    for (size_t i = 0; i < totalMemory / frameSize; ++i) {
        size_t startAddress = i * frameSize;
        std::shared_ptr<AttachedProcess> process = memoryAllocator->getPartitionAt(startAddress).process;

        if (process) {
            size_t endAddress = startAddress + frameSize;
            output << endAddress << "\n";          
            output << "P" << process->getPid() << "\n"; 
            output << startAddress << "\n";        
        }
        else {
            output << startAddress + frameSize << "\n";
            output << "\n"; 
            output << startAddress << "\n"; 
            externalFragmentation += frameSize; 
        }
    }
    output << "----start---- = 0\n";
    output << "Total external fragmentation in KB: " << (externalFragmentation / 1024) << "\n";

    return output.str();
}
