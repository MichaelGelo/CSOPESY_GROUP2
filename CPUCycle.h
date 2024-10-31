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

    std::mutex& getMutex() { return mtx; }
    std::condition_variable& getConditionVariable() { return cv; }
    bool isRunning() const { return running; }
    mutable std::mutex mtx;
    std::condition_variable cv;      

private:
    void runCycles();          

    int cycleCount;                  
    bool running;                   
    std::thread clockThread;          
               

};
