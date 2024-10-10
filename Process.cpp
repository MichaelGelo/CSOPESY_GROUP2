#include "Process.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <thread>
#include <chrono>

Process::Process(int pid, std::string screenName, int core, int maxLines)
    : pid(pid), screenName(screenName), core(core), maxLines(maxLines), curLines(0), state(READY), isFinished(false) {}

Process::ProcessState Process::getState() const {
    return state;
}

void Process::switchState(ProcessState newState) {
    state = newState;
}

bool Process::hasFinished() const {
    return isFinished;
}

void Process::executeCommand(std::function<void()> command) {
    if (state == RUNNING) {
        for (curLines = 0; curLines < maxLines; ++curLines) {
            command();
            logPrintCommand("Hello world from " + screenName + "!");
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Sleep 
        }
        switchState(FINISHED);
        isFinished = true;
        std::cout << "Process " << screenName << " finished executing command." << std::endl;
    }
    else {
        std::cout << "Cannot execute command. Process is not in RUNNING state." << std::endl;
    }
}

void Process::logPrintCommand(const std::string& message) {
    std::ofstream logFile(screenName + ".txt", std::ios::app);

    if (logFile.is_open()) {
        auto t = std::time(nullptr);
        std::tm localTime;
        localtime_s(&localTime, &t); 

        logFile << "(" << std::put_time(&localTime, "%m/%d/%Y %I:%M:%S%p") << ") "
            << "Core:" << core << " \"" << message << "\"" << std::endl;
    }
    logFile.close();
}

void Process::displayProcessInfo() const {
    std::cout << "Process ID: " << pid << "\n"
        << "Screen Name: " << screenName << "\n"
        << "Assigned Core: " << core << "\n"
        << "Current State: " << (state == READY ? "READY" :
            state == RUNNING ? "RUNNING" :
            state == WAITING ? "WAITING" : "FINISHED")
        << "\nFinished: " << (isFinished ? "Yes" : "No") << std::endl;
}

int Process::getPid() const {
    return pid;
}

const std::string& Process::getScreenName() const {
    return screenName;
}

int Process::getCore() const {
    return core;
}
