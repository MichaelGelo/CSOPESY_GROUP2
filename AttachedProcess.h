#ifndef ATTACHEDPROCESS_H
#define ATTACHEDPROCESS_H
#include <iostream>

#include "Process.h"

class AttachedProcess : public Process {
public:
    AttachedProcess(int pid, std::string screenName, int core, int maxLines, int memoryRequirement)
        : Process(pid, screenName, core, maxLines, memoryRequirement), memoryLocation(nullptr) {}

    void* getMemoryLocation() const override {
        // std::cout << "Retrieving memory location for process " << getPid() << ": " << memoryLocation << std::endl;
        return memoryLocation;
    }

    void setMemoryLocation(void* location) {
        memoryLocation = location;
    }




private:
    void* memoryLocation;
};

#endif
