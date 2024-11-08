#pragma once
#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <functional>
#include <mutex>
#include <memory>        
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

    void* memoryPointer = nullptr;

public:
    enum ProcessState {
        READY,
        RUNNING,
        WAITING,
        FINISHED
    };

private:
    ProcessState state;
    typedef std::vector<std::shared_ptr<ICommand>> CommandList;
    CommandList commandList;

public:
    Process(int pid, std::string screenName, int core, int maxLines, int memoryRequirement);
    void generateCommands();
    Process(const Process&) = delete;
    Process& operator=(const Process&) = delete;
    void logPrintCommand(const std::string& message);

    Process(Process&& other) noexcept
        : pid(other.pid), screenName(std::move(other.screenName)),
        core(other.core), maxLines(other.maxLines),
        curLines(other.curLines), isFinished(other.isFinished),
        state(other.state), memoryRequirement(other.memoryRequirement) {
        other.pid = -1;
        other.isFinished = false;
    }


    Process& operator=(Process&& other) noexcept {
        if (this != &other) {
            pid = other.pid;
            screenName = std::move(other.screenName);
            core = other.core;
            maxLines = other.maxLines;
            curLines = other.curLines;
            isFinished = other.isFinished;
            state = other.state;
            memoryRequirement = other.memoryRequirement; 

            other.pid = -1;
            other.isFinished = false;
        }
        return *this;
    }

    void setMemoryPointer(void* location) {
        memoryPointer = location;
    }

    int getPid() const;
    const std::string& getScreenName() const;
    int getCore() const;
    int getCurLines() const;
    int getMaxLines() const;
    int getRemainingInstructions() const;
    ProcessState getState() const;
    bool hasFinished() const;
    void executeCommand();
    void getNextCommand(std::function<void()> command);
    void switchState(ProcessState newState);
    void displayProcessInfo() const;
    void setCore(int coreID);
    int getMemoryRequirement() const;
    virtual ~Process() = default;

};

#endif 