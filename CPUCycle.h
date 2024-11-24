#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>

class CPUCycle {
public:
    CPUCycle();                       
    ~CPUCycle();                      // Destructor?

    void startClock();               
    void stopClock();                 
    int getCurrentCycle() const;     
    void setCycleDelay(int delayMicroseconds);
    int getCycleDelay() const;

    std::mutex& getMutex() { return mtx; }
    std::condition_variable& getConditionVariable() { return cv; }
    bool isRunning() const { return running; }
    mutable std::mutex mtx;
    std::condition_variable cv;      

    int getActiveCycleCount() const;  // Active ticks
    void incrementActiveCycle();  // Increment active ticks
private:
    void runCycles();          
    std::atomic<int> activeCycleCount;  // Active ticks
    std::atomic<int> cycleCount;
    bool running;                   
    std::thread clockThread;        
    int cycleDelay;
               
};
