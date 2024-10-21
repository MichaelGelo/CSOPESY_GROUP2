#ifndef PROCESS_H
#define PROCESS_H

#include <string>

class Process {
public:
    Process(const std::string& name) : processName(name) {}

    std::string getProcessName() const {
        return processName;
    }

private:
    std::string processName;
};

#endif
