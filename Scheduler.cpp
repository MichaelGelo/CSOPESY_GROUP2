#include <string>
#include <vector>
#include <iostream>
#include "Scheduler.h"

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

void Scheduler::fcfs() {

}

void Scheduler::rr() {

}

void Scheduler::schedulerTest() {

}

void Scheduler::schedulerStop() {

}
