#pragma once
#include "Process.h"
#include "ConsoleManager.h"
#include <string>
#include <vector>
#include <iostream>

class Scheduler {
private:
    int numCpu;
    std::string schedulerAlgorithm;
    int quantumCycles;
    int batchProcessFreq;
    int minInstructions;
    int maxInstructions;
    int delayPerExec;
    bool schedulerStatus = false;

    void fcfs();
    void rr();

public:
    Scheduler(int numCpu, const std::string& scheduler, int quantumCycles, int batchProcessFreq,
        int minIns, int maxIns, int delayExec)
        : numCpu(numCpu), schedulerAlgorithm(scheduler), quantumCycles(quantumCycles),
        batchProcessFreq(batchProcessFreq), minInstructions(minIns),
        maxInstructions(maxIns), delayPerExec(delayExec) {}

    const std::string& getSchedulerAlgorithm() const { return schedulerAlgorithm; }
    int getQuantumCycles() const { return quantumCycles; }
    int getMinIns() const { return minInstructions; }
    int getMaxIns() const { return maxInstructions; }
    int getNumCpu() const { return numCpu; }

    void schedulerTest();
    void displayConfiguration();
    void schedulerStop();

};