#pragma once
#include <atomic>
#include <thread>
#include <mutex>
#include <chrono>

class CPUCycle {
public:
    CPUCycle();
    ~CPUCycle();

    void startClock();
    void stopClock();
    int getCurrentCycle() const;
    bool isRunning() const;

private:
    std::atomic<int> cycleCount;
    std::atomic<bool> running;
    std::thread clockThread;
    std::mutex mtx;
};