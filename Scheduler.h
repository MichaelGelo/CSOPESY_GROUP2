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

    void schedulerTest();
    void displayConfiguration();
    void schedulerStop();
};