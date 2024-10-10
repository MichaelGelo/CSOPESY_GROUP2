#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <fstream>
#include <ctime>
#include <iostream>
#include <functional>

class Process {
public:
    enum ProcessState { READY, RUNNING, WAITING, FINISHED };

    Process(int pid, std::string screenName, int core, int maxLines);
    ProcessState getState() const;
    void switchState(ProcessState newState);
    bool hasFinished() const;
    void executeCommand(std::function<void()> command);
    void displayProcessInfo() const;
    void logPrintCommand(const std::string& message);

    int getPid() const;
    const std::string& getScreenName() const;
    int getCore() const;

private:
    int pid;
    std::string screenName;
    int core;
    int maxLines;
    int curLines;
    ProcessState state;
    bool isFinished;
};

#endif // PROCESS_H
