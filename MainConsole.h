#ifndef MAIN_CONSOLE_H
#define MAIN_CONSOLE_H

#include "AConsole.h"
#include <vector>
#include <string>
#include <memory>
#include <map>           // Corrected inclusion of <map>
#include <functional>
#include "ScreenConsole.h"
#include "Process.h"
#include "Scheduler.h"
#include "CPUCycle.h"

class MainConsole : public AConsole {

private:
    void captureAndStoreOutput(std::function<void()> func);
    std::vector<std::string> commandHist;

    void showHistory() const;
    bool menuShown;
    void clear();  
    void saveProcessReport() const;
    std::map<std::string, Process*> processQueue;  // Corrected map inclusion

    // for cpu cycle
    std::unique_ptr<CPUCycle> cpuCycleCounter;
    bool isCPURunning;

    // Add configuration members
    struct Configuration {
        int numCpu;
        std::string scheduler;
        int quantumCycles;
        int batchProcessFreq;
        int minIns;
        int maxIns;
        int delayPerExec;

        // Constructor with default values
        Configuration() :
            numCpu(0),
            scheduler(""),
            quantumCycles(0),
            batchProcessFreq(0),
            minIns(0),
            maxIns(0),
            delayPerExec(0) {}
    };

    Configuration config;

    void createProcess(std::string processName);

public:
    MainConsole();
    void testCpuCore();
    void onEnabled() override;
    void display() override;
    void process() override;
    void menu() const;
    void enter() const;
    void color(int n) const;
    bool isInitialized;
    std::shared_ptr<Scheduler> schedulerInstance;

    std::vector<std::shared_ptr<Process>> processes;
    int nextPid = 1;
    void displayProcessStatus() const;
    const Configuration& getConfiguration() const { return config; }
};

#endif // MAIN_CONSOLE_H