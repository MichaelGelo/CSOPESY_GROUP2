#pragma once
#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <functional>
#include <mutex>
#include <memory>
#include <vector>
#include "ICommand.h"

class Process {
private:
    int pid;
    std::string screenName;
    int core;
    int maxLines;
    int curLines;
    bool isFinished;
    mutable std::mutex mutex;
    std::string finishedTime;
    int memoryRequirement;

    int memPerFrame; // Memory per frame
    int minMemPerProc; // Minimum memory per process
    int maxMemPerProc; // Maximum memory per process

    void* memoryPointer = nullptr;
    bool isAllocated;

    int P = 0;
    int M = 0;

public:
    enum ProcessState {
        READY,
        RUNNING,
        WAITING,
        FINISHED
    };

private:
    ProcessState state;
    using CommandList = std::vector<std::shared_ptr<ICommand>>;
    CommandList commandList;

public:
    Process(int pid, std::string screenName, int core, int maxLines, int memoryRequirement, int memPerFrame, int minMemPerProc, int maxMemPerProc);

    Process(const Process&) = delete;
    Process& operator=(const Process&) = delete;

    Process(Process&& other) noexcept;
    Process& operator=(Process&& other) noexcept;

    void generateCommands();
    void executeCommand();
    void getNextCommand(std::function<void()> command);
    void logPrintCommand(const std::string& message);
    void displayProcessInfo(const std::string& message) const;

    ProcessState getState() const;
    void switchState(ProcessState newState);
    bool hasFinished() const;

    int getPid() const;
    const std::string& getScreenName() const;
    int getCore() const;
    int getCurLines() const;
    int getMaxLines() const;
    int getRemainingInstructions() const;
    int getMemoryRequirement() const;

    void setCore(int coreID);

    virtual void* getMemoryLocation() const { return memoryPointer; }
    virtual void setMemoryLocation(void* location) { memoryPointer = location; }
    bool hasAllocated() const;
    void allocateResources();
    void deallocateResources();
    void calculateMandP();
    virtual ~Process() = default;

    int getM() const {
        return M;
    }

    int getP() const {
        return P;
    }
};

#endif
