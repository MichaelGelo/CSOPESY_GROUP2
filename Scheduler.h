#pragma once
#include "Process.h"
#include "ConsoleManager.h"
#include "FlatMemoryAllocator.h"
#include "AttachedProcess.h"
#include "CPUCycle.h"
#include "CPUCore.h"
#include "Frame.h"
#include <string>
#include <queue>
#include <vector>
#include <iostream>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <memory>
//
class PagingMemoryAllocator;

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

    int numFrames;

    std::atomic<bool> schedulerStatus = false;

    std::vector<std::unique_ptr<CPUCore>> cores;  // Use unique_ptr to manage CPUCore objects
    std::vector<std::thread> coreThreads;         // Threads for each core
    std::queue<std::shared_ptr<Process>> rq;      // Ready queue
    std::mutex mtx;                               // General mutex
    std::condition_variable cv;                   // Condition variable for scheduler
    std::mutex rqMutex;                           // Mutex for ready queue
    std::condition_variable rqCondition;          // Condition variable for ready queue
    std::shared_ptr<FlatMemoryAllocator> memoryAllocator;

    std::queue<std::shared_ptr<Frame>> frameQueue;
    std::mutex frameQueueMutex;

    // Private helper methods
    void initializeCores();                       // Initializes CPU cores and threads
    void removeQuotes(std::string& str);
    void fcfs();                                  // Implements First-Come, First-Served algorithm
    void rr();                                    // Implements Round Robin algorithm
    void listenForCycle();                        // Helper function to manage cycle-based scheduling
    void listenForCycleRR();
    void stopAllCores();                          // Stops all cores and joins threads

    
    int calculateProcessesInMemory();
    int calculateExternalFragmentation();
    std::string getMemoryPrintout();

    friend class PagingMemoryAllocator;
    
public:
    // Constructor
    Scheduler(CPUCycle& cpuCycle, int numCpu, const std::string& scheduler, int quantumCycles,
        int batchProcessFreq, int minIns, int maxIns, int delayExec,
        int maxOverallMem, int memPerFrame, int minMemPerProc, int maxMemPerProc)
        : cpuCycle(cpuCycle), numCpu(numCpu), schedulerAlgorithm(scheduler), quantumCycles(quantumCycles),
        batchProcessFreq(batchProcessFreq), minInstructions(minIns), maxInstructions(maxIns),
        delayPerExec(delayExec), maxOverallMem(maxOverallMem), memPerFrame(memPerFrame),
        minMemPerProc(minMemPerProc), maxMemPerProc(maxMemPerProc) {

        if (memPerFrame > 0) {
            numFrames = maxOverallMem / memPerFrame;
        }
        else {
            numFrames = 0; 
        }

        for (int i = 0; i < numFrames; ++i) {
            frameQueue.emplace(std::make_shared<Frame>(i, memPerFrame, false));

        }

        std::lock_guard<std::mutex> lock(frameQueueMutex); // Ensure thread-safe access to the frameQueue
        std::queue<std::shared_ptr<Frame>> tempQueue = frameQueue; // Make a copy to avoid modifying the original queue

        std::cout << "Frames in queue:\n";
        while (!tempQueue.empty()) {
            auto frame = tempQueue.front();
            tempQueue.pop();
            std::cout << "Frame Number: " << frame->getFrameNum()
                << ", Memory Per Frame: " << frame->getMemPerFrame()
                << std::endl;
        }

        memoryAllocator = std::make_shared<FlatMemoryAllocator>(maxOverallMem, memPerFrame, minMemPerProc, maxMemPerProc);
        memoryAllocator->printConfiguration(); // DEBUGGING PURPOSES, REMOVE.

        initializeCores();
        removeQuotes(schedulerAlgorithm);

        if (schedulerAlgorithm == "fcfs") {
            std::cout << "Using First-Come, First-Served (FCFS) scheduling algorithm." << std::endl;
            //std::cout << "Number of Frames: " << numFrames << std::endl;

            fcfs();
        }
        else if (schedulerAlgorithm == "rr") {
            std::cout << "Using Round Robin (RR) scheduling algorithm." << std::endl;
            //std::cout << "Number of Frames: " << numFrames << std::endl;

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

    void generateMemoryReport(int currentQuantumCycle);

};
