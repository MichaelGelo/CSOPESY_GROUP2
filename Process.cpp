#include "Process.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <thread>
#include <chrono>
#include <fstream>
#include <random>
#include <stdexcept>


Process::Process(int pid, std::string screenName, int core, int maxLines, int memoryRequirement, int memPerFrame, int minMemPerProc, int maxMemPerProc)
    : pid(pid),
    screenName(std::move(screenName)),
    core(core),
    maxLines(maxLines),
    curLines(0),
    isFinished(false),
    state(READY),
    memoryRequirement(memoryRequirement),
    memPerFrame(memPerFrame),
    minMemPerProc(minMemPerProc),
    maxMemPerProc(maxMemPerProc),
    isAllocated(false),
    M(0), // Initialize M to a default value
    P(0)  // Initialize P to a default value
{
    calculateMandP(); 
}

Process::Process(Process&& other) noexcept
    : pid(other.pid),
    screenName(std::move(other.screenName)),
    core(other.core),
    maxLines(other.maxLines),
    curLines(other.curLines),
    isFinished(other.isFinished),
    state(other.state),
    memoryRequirement(other.memoryRequirement),
    memPerFrame(other.memPerFrame),
    minMemPerProc(other.minMemPerProc),
    maxMemPerProc(other.maxMemPerProc),
    memoryPointer(other.memoryPointer),
    isAllocated(other.isAllocated),
    M(other.M),
    P(other.P)
{
    other.pid = -1;
    other.isFinished = false;
    other.memoryPointer = nullptr;
    other.isAllocated = false;
}

Process& Process::operator=(Process&& other) noexcept {
    if (this != &other) {
        pid = other.pid;
        screenName = std::move(other.screenName);
        core = other.core;
        maxLines = other.maxLines;
        curLines = other.curLines;
        isFinished = other.isFinished;
        state = other.state;
        memoryRequirement = other.memoryRequirement;
        memPerFrame = other.memPerFrame;
        minMemPerProc = other.minMemPerProc;
        maxMemPerProc = other.maxMemPerProc;
        memoryPointer = other.memoryPointer;
        isAllocated = other.isAllocated;
        M = other.M;
        P = other.P;

        other.pid = -1;
        other.isFinished = false;
        other.memoryPointer = nullptr;
        other.isAllocated = false;
    }
    return *this;
}



Process::ProcessState Process::getState() const {
    return state;
}

void Process::switchState(ProcessState newState) {
    state = newState;
}

bool Process::hasFinished() const {
    return isFinished;
}

void Process::generateCommands() {
    for (int i = 0; i < maxLines; ++i) {
        auto command = std::make_shared<ICommand>(pid, ICommand::IO);

        // Add the command to the command list
        commandList.push_back(command);
    }
}

void Process::executeCommand() {
    if (curLines < commandList.size()) {
        // Execute the current command in the list
        commandList[curLines]->execute();
    }

    // If all commands have been executed, mark the process as finished
    if (curLines >= maxLines || curLines >= commandList.size()) {
        isFinished = true;
        switchState(FINISHED);
    }

    time_t now = time(0);
    tm localtm;
    localtime_s(&localtm, &now);
    char timeBuffer[50];
    strftime(timeBuffer, sizeof(timeBuffer), "(%m/%d/%Y %I:%M:%S %p)", &localtm);
    finishedTime = timeBuffer;
}


void Process::getNextCommand(std::function<void()> command) {
    if (curLines < maxLines) {

        curLines++;
    }
    else {
        switchState(FINISHED);
    }
}

void Process::logPrintCommand(const std::string& message) {
    std::ofstream logFile(screenName + ".txt", std::ios::app);
    if (logFile.is_open()) {
        // Check if the file is empty
        logFile.seekp(0, std::ios::end);
        if (logFile.tellp() == 0) {
            // If the file is empty, write the title
            logFile << "Process name: " << screenName << std::endl;
            logFile << "Logs:" << std::endl << std::endl;
        }

        auto t = std::time(nullptr);
        std::tm localTime;
        localtime_s(&localTime, &t);
        logFile << "(" << std::put_time(&localTime, "%m/%d/%Y %I:%M:%S%p") << ") "
            << "Core:" << core << " \"" << message << "\"" << std::endl;
    }
    logFile.close();
}

void Process::displayProcessInfo(const std::string& message) const {
    std::ostringstream timeStream;
    if (isFinished) {
        timeStream << finishedTime;

        std::cout << std::left << std::setw(10) << screenName  // Set width for name
            << "  " << std::setw(28) << timeStream.str()  // Set width for timestamp
            << "  Finished "
            << "  " << curLines << "/" << maxLines;
                  
        if (message == "process-smi") {
            std::cout  << "  Memory Usage: " << memoryRequirement << std::endl;
        }
        else {
            std::cout << std::endl;
        }
    }
    else {
        time_t now = time(0);
        tm localtm;
        localtime_s(&localtm, &now);
        char timeBuffer[50];
        strftime(timeBuffer, sizeof(timeBuffer), "(%m/%d/%Y %I:%M:%S %p)", &localtm);
        std::ostringstream timeStream;
        timeStream << timeBuffer;

        std::cout << std::left << std::setw(10) << screenName  // Set width for name
            << "  " << std::setw(28) << timeStream.str()  // Set width for timestamp
            << "  Core: " << std::setw(3) << core        // Set width for core
            << "  " << std::right << std::setw(3) << curLines << "/" << std::left << std::setw(3) << maxLines;
        if (message == "process-smi") {
            std::cout << "  Memory Usage: " << memoryRequirement << std::endl;
        }
        else {
            std::cout << std::endl;
        }
    }
}


void Process::setCore(int coreID) {
   core = coreID;
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

int Process::getCurLines() const {
    return curLines;
}

int Process::getMaxLines() const {
    return maxLines;
}

int Process::getRemainingInstructions() const {
    return maxLines - curLines;
}

int Process::getMemoryRequirement() const {
    return memoryRequirement;
}

bool Process::hasAllocated() const {
    return isAllocated;
}

void Process::allocateResources() {
    isAllocated = true;
}

void Process::deallocateResources() {
    isAllocated = false;
}

void Process::calculateMandP() {
    std::random_device rd;
    std::mt19937 gen(rd());

    int minPow = static_cast<int>(std::log2(minMemPerProc));
    int maxPow = static_cast<int>(std::log2(maxMemPerProc));

    std::uniform_int_distribution<> dis(minPow, maxPow);
    M = 1 << dis(gen); 
    if (memPerFrame > 0) {
        P = (M + memPerFrame - 1) / memPerFrame;
    }
    else {
        P = 0;
    }
}