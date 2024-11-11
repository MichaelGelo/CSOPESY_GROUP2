#pragma once
#include "Process.h"
#include "ConsoleManager.h"
#include "FlatMemoryAllocator.h"
#include "AttachedProcess.h"
#include "CPUCycle.h"
#include "CPUCore.h"
#include <string>
#include <queue>
#include <vector>
#include <iostream>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <memory>

class Scheduler {
private:
    CPUCycle& cpuCycle;
    int numCpu;
    std::string schedulerAlgorithm;
    int quantumCycles;
    int batchProcessFreq;
    int minInstructions;
    int maxInstructions;
    int delayPerExec;

    // added for mo2
    int quantumCycleCounter;
    int maxOverallMem;
    int memPerFrame;
    int minMemPerProc;
    int maxMemPerProc;

    std::atomic<bool> schedulerStatus = false;

    std::vector<std::unique_ptr<CPUCore>> cores;  // Use unique_ptr to manage CPUCore objects
    std::vector<std::thread> coreThreads;         // Threads for each core
    std::queue<std::shared_ptr<Process>> rq;      // Ready queue
    std::mutex mtx;                               // General mutex
    std::condition_variable cv;                   // Condition variable for scheduler
    std::mutex rqMutex;                           // Mutex for ready queue
    std::condition_variable rqCondition;          // Condition variable for ready queue
    std::shared_ptr<FlatMemoryAllocator> memoryAllocator;

    // Private helper methods
    void initializeCores();                       // Initializes CPU cores and threads
    void removeQuotes(std::string& str);
    void fcfs();                                  // Implements First-Come, First-Served algorithm
    void rr();                                    // Implements Round Robin algorithm
    void listenForCycle();                        // Helper function to manage cycle-based scheduling
    void listenForCycleRR();
    void stopAllCores();                          // Stops all cores and joins threads

    void generateMemoryReport(int currentQuantumCycle);
    int calculateProcessesInMemory();
    int calculateExternalFragmentation();
    std::string getMemoryPrintout();
public:
    // Constructor
    Scheduler(CPUCycle& cpuCycle, int numCpu, const std::string& scheduler, int quantumCycles,
        int batchProcessFreq, int minIns, int maxIns, int delayExec,
        int maxOverallMem, int memPerFrame, int minMemPerProc, int maxMemPerProc)
        : cpuCycle(cpuCycle), numCpu(numCpu), schedulerAlgorithm(scheduler), quantumCycles(quantumCycles),
        batchProcessFreq(batchProcessFreq), minInstructions(minIns), maxInstructions(maxIns),
        delayPerExec(delayExec), maxOverallMem(maxOverallMem), memPerFrame(memPerFrame),
        minMemPerProc(minMemPerProc), maxMemPerProc(maxMemPerProc) {

        memoryAllocator = std::make_shared<FlatMemoryAllocator>(maxOverallMem, memPerFrame, minMemPerProc);
        memoryAllocator->printConfiguration(); // DEBUGGING PURPOSES, REMOVE.

        initializeCores();
        removeQuotes(schedulerAlgorithm);

        if (schedulerAlgorithm == "fcfs") {
            std::cout << "Using First-Come, First-Served (FCFS) scheduling algorithm." << std::endl;
            fcfs();
        }
        else if (schedulerAlgorithm == "rr") {
            std::cout << "Using Round Robin (RR) scheduling algorithm." << std::endl;
            rr();
        }
        else {
            std::cout << "Unknown scheduling algorithm specified." << std::endl;
        }
    }

    ~Scheduler(); 

    const std::string& getSchedulerAlgorithm() const { return schedulerAlgorithm; }
    int getQuantumCycles() const { return quantumCycles; }
    int getMinIns() const { return minInstructions; }
    int getMaxIns() const { return maxInstructions; }
    int getNumCpu() const { return numCpu; }
    CPUCycle& getCpuCycle() { return cpuCycle; }
    bool isRoundRobin() const { return schedulerAlgorithm == "rr"; }

    //void schedulerTest();                           
    void displayConfiguration();                    
    void schedulerStop();                           
    void addToRQ(std::shared_ptr<AttachedProcess> process);
    bool attachProcessToMemory(std::shared_ptr<AttachedProcess>& process);

};
