#pragma once
#include "AConsole.h"
#include <string>
#include <vector>
#include <memory>
#include "Process.h"

class ScreenConsole : public AConsole {
public:
    ScreenConsole(const std::string& processName, int pid, int core, int totalLines, std::vector<std::shared_ptr<Process>>& processes);
    std::shared_ptr<Process> getProcess() const;

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
    std::shared_ptr<Process> processInstance;               // changed the name para walang error sa void process();
    std::vector<std::shared_ptr<Process>>& processes; 

    std::vector<std::string> commandHist;        
    std::string timestamp;                      
    bool isActive = false;                      
    std::string lastCommand;                    

    // Command-specific handling
    void handleProcessSmi();
};
