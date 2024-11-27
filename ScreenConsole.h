#pragma once
#include "AConsole.h"
#include <string>
#include <vector>
#include <memory>
#include "AttachedProcess.h"

class ScreenConsole : public AConsole {
public:
    ScreenConsole(const std::string& processName, int pid, int core, int totalLines,
        int memoryRequirement, int memPerFrame, int minMemPerProc, int maxMemPerProc,
        std::vector<std::shared_ptr<AttachedProcess>>& processes);

    std::shared_ptr<AttachedProcess> getProcess() const;

    void onEnabled() override;
    void display() override;
    void process() override;

private:
    std::string getCurrentTimestamp() const;

    std::string processName;
    int pid;
    int core;
    int totalLines;
    int currentLine = 0;
    int memPerFrame;       
    int minMemPerProc;     
    int maxMemPerProc;
    std::shared_ptr<AttachedProcess> processInstance; 
    std::vector<std::shared_ptr<AttachedProcess>>& processes; 

    std::vector<std::string> commandHist;
    std::string timestamp;
    bool isActive = false;
    std::string lastCommand;

    void handleProcessSmi();
};
