#pragma once
#include <atomic>
#include <thread>
#include <mutex>
#include <chrono>

class CPUCycle {
public:
    CPUCycle();
    ~CPUCycle();

    void startClock(std::string scheduler, int delayPerExec, int numCpu, int s);
    void stopClock();
    int getCurrentCycle() const;
    bool isRunning() const;

private:
    std::string scheduler;
    int delayPerExec;
    std::atomic<int> cycleCount;
    std::atomic<bool> running;
    std::thread clockThread;
    std::mutex mtx;
};