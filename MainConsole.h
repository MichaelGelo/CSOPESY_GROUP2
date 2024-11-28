#ifndef MAIN_CONSOLE_H
#define MAIN_CONSOLE_H

#include "AConsole.h"
#include <vector>
#include <string>
#include <memory>
#include <map> 
#include <functional>
#include "ScreenConsole.h"
#include "AttachedProcess.h"
#include "Scheduler.h"
#include "CPUCycle.h"
//#include "PagingMemoryAllocator.h"
class MainConsole : public AConsole {

private:
    PagingMemoryAllocator* paging;
    void captureAndStoreOutput(std::function<void()> func);
    std::vector<std::string> commandHist;

    void showHistory() const;
    bool menuShown;
    void clear();
    void saveProcessReport() const;
    void processSMI() const;
    void vmstat() const;
    std::map<std::string, AttachedProcess*> processQueue;

    std::vector<std::thread> cpuThreads;
    std::mutex processMutex;
    std::condition_variable processCV;
    bool isSchedulerTestRunning = false;
    std::thread testThread;
    void runSchedulerTest();

    // CPU cycle management
    std::unique_ptr<CPUCycle> cpuCycleCounter;
    bool isCPURunning;

    // Configuration for the scheduler
    struct Configuration {
        int numCpu;
        std::string scheduler;
        int quantumCycles;
        int batchProcessFreq;
        int minIns;
        int maxIns;
        int delayPerExec;
        int maxOverallMem;
        int memPerFrame;
        int minMemPerProc;
        int maxMemPerProc;

        Configuration() :
            numCpu(0),
            scheduler(""),
            quantumCycles(0),
            batchProcessFreq(0),
            minIns(0),
            maxIns(0),
            delayPerExec(0),
            maxOverallMem(0),
            memPerFrame(0),
            minMemPerProc(0),
            maxMemPerProc(0) {}
    };

    Configuration config;

    void createProcess(std::string processName);

    // report utility
    std::vector<std::string> screenListHist;

public:
    MainConsole();
    void onEnabled() override;
    void display() override;
    void process() override;
    void menu() const;
    void enter() const;
    bool isInitialized;
    std::shared_ptr<Scheduler> schedulerInstance;
    CPUCycle cpuCycle;

    // Update to use AttachedProcess instead of Process
    std::vector<std::shared_ptr<AttachedProcess>> processes;
    int nextPid = 1;

    bool isPaging;

    void displayProcessStatus() const;
    const Configuration& getConfiguration() const { return config; }

    void schedulerTest();
    void schedulerStop();
};

#endif // MAIN_CONSOLE_H
