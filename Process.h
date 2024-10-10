#pragma once
#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <functional>

class Process {
private:
    int pid;
    std::string screenName;
    int curLines;
    int maxLines;
    int core;
    bool isFinished;

public:
    enum ProcessState {
        READY,
        RUNNING,
        WAITING,
        FINISHED
    };

private:
    ProcessState state;  

public:
    Process(int pid, std::string screenName, int core, int maxLines);

    ProcessState getState() const;

    void switchState(ProcessState newState);

    bool hasFinished() const;

    void executeCommand(std::function<void()> command);

    void displayProcessInfo() const;
};

#endif 
