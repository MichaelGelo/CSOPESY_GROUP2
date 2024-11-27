#pragma once
#ifndef PAGE_H
#define PAGE_H

#include "Process.h"
#include <iostream>

class Page {
private:
    int pid;                 
    int memoryPerPage;       

public:
    Page(int pid, int memoryRequirement, int numPages)
        : pid(pid),
        memoryPerPage(numPages > 0 ? memoryRequirement / numPages : 0) {
    }

    int getPid() const {
        return pid;
    }

    int getMemoryPerPage() const {
        return memoryPerPage;
    }

    void setPid(int newPid) {
        pid = newPid;
    }

    void setMemoryPerPage(int memory) {
        memoryPerPage = memory;
    }

    void displayPageInfo() const {
        std::cout << "Page Info - PID: " << pid
            << ", Memory Per Page: " << memoryPerPage << " bytes"
            << std::endl;
    }
};

#endif
