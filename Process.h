#pragma once
#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <functional>
#include <mutex>

class Process {
private:
    int pid;
    std::string screenName;
    int core;
    int maxLines;
    int curLines;
    bool isFinished;
    mutable std::mutex mutex;

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

    Process(const Process&) = delete;
    Process& operator=(const Process&) = delete;
    void logPrintCommand(const std::string& message);

    Process(Process&& other) noexcept
        : pid(other.pid), screenName(std::move(other.screenName)),
        core(other.core), maxLines(other.maxLines),
        curLines(other.curLines), isFinished(other.isFinished),
        state(other.state) {
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

            other.pid = -1;
            other.isFinished = false;
        }
        return *this;
    }

    int getPid() const;
    const std::string& getScreenName() const;
    int getCore() const;
    ProcessState getState() const;
    bool hasFinished() const;
    void executeCommand(std::function<void()> command);
    void switchState(ProcessState newState);
    void displayProcessInfo() const;
    void run();
};

#endif 