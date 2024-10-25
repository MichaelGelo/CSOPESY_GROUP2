#ifndef CPU_CORE_H
#define CPU_CORE_H

#include "Process.h"
#include "Scheduler.h"
#include <queue>
#include <memory>
#include <chrono>
#include <thread>
#include <string>

// Enum to define different scheduling modes
enum class SchedulingMode {
    FCFS,
    ROUND_ROBIN
};

class CpuCore {
public:
    // Corrected class name and constructor name
    CpuCore(int coreNumber, std::shared_ptr<Scheduler> scheduler);

    void addProcess(std::shared_ptr<Process> process);
    void execute();
    bool hasProcesses() const;

private:
    int coreNumber;
    int quantumTime;
    SchedulingMode mode;
    std::queue<std::shared_ptr<Process>> processQueue;
    std::shared_ptr<Scheduler> schedulerInstance;  // Store reference to Scheduler

    void runFCFS();
    void runRoundRobin();
};

#endif // CPU_CORE_H
