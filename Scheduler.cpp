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


// sa labas
//gagawa ng cores based sa numCPU

void Scheduler::fcfs() {
    // kada notify, check if may laman rq
    //check if may open na cpuCore
    //if meron, lagay yung process sa cpuCore
    
}


void Scheduler::rr() {

}

void Scheduler::schedulerTest() {

}

void Scheduler::schedulerStop() {

}


//for cpuCore
//check if may laman siya na process
//if notify, run si Process executeCommand() and getNextCommand()
//if current lines == max lines or done na ung process, tanggalin na

// quantum ni kai for round robin
