#include "Process.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <thread>
#include <chrono>
#include <fstream>

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
        // Print 100 lines regardless of maxLines
        for (int i = 0; i < 100; ++i) {
            command();
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Sleep
        }
        switchState(FINISHED);
        isFinished = true;
    }
    else {
        std::cout << "Cannot execute command. Process is not in RUNNING state." << std::endl;
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

void Process::displayProcessInfo() const {
    if (isFinished) {
        time_t now = time(0);
        tm localtm;
        localtime_s(&localtm, &now);
        char timeBuffer[50];
        strftime(timeBuffer, sizeof(timeBuffer), "(%m/%d/%Y %I:%M:%S %p)", &localtm);
        std::ostringstream timeStream;
        timeStream << timeBuffer;

        std::cout << std::left << std::setw(10) << screenName  // Set width for name
            << "  " << std::setw(28) << timeStream.str()       // Set width for timestamp
            << "  Finished "
            << "  " << curLines << "/" << maxLines;
        std::cout << std::endl;
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
            << "  " << std::setw(28) << timeStream.str()       // Set width for timestamp
            << "  Core: " << std::setw(3) << core              // Set width for core
            << "  " << std::right << std::setw(3) << curLines << "/" << std::left << std::setw(3) << maxLines;
        std::cout << std::endl;
    }
}

void Process::run() {
    switchState(RUNNING);
    executeCommand([this]() {
        // Only increment curLines if it is less than maxLines
        if (curLines < maxLines) {
            curLines++;
        }
        //std::cout << pid << " executed line " + std::to_string(curLines) + " / " + std::to_string(maxLines) + "\n";
        logPrintCommand("Hello world from " + screenName + "!");

        });
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
