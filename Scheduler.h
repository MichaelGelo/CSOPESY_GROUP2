#pragma once
#include "Process.h"
#include "ConsoleManager.h"
#include "CPUCycle.h"
#include "CPUCore.h"
#include <string>
#include <queue>
#include <vector>
#include <iostream>
#include <condition_variable>
#include <mutex>
#include <thread>

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
    bool schedulerStatus = false;

    std::vector<CPUCore> cores;
    std::queue<std::unique_ptr<Process>> rq;
    std::mutex mtx;
    std::condition_variable cv;

    void removeQuotes(std::string& str);
    void fcfs();
    void rr();
    void initializeCores();
    void listenForCycle(); 

public:
    Scheduler(CPUCycle& cpuCycle, int numCpu, const std::string& scheduler, int quantumCycles,
        int batchProcessFreq, int minIns, int maxIns, int delayExec)
        : cpuCycle(cpuCycle), numCpu(numCpu), schedulerAlgorithm(scheduler), quantumCycles(quantumCycles),
        batchProcessFreq(batchProcessFreq), minInstructions(minIns), maxInstructions(maxIns),
        delayPerExec(delayExec) { // added cpuCycle para synchronized

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


    const std::string& getSchedulerAlgorithm() const { return schedulerAlgorithm; }
    int getQuantumCycles() const { return quantumCycles; }
    int getMinIns() const { return minInstructions; }
    int getMaxIns() const { return maxInstructions; }
    int getNumCpu() const { return numCpu; }

    void schedulerTest();
    void displayConfiguration();
    void schedulerStop();
};

