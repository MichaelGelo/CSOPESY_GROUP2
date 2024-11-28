#include "PagingMemoryAllocator.h"
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <iomanip>

PagingMemoryAllocator::PagingMemoryAllocator(size_t totalMemory, size_t frameSize, Scheduler& scheduler)
    : totalMemory(totalMemory), frameSize(frameSize), scheduler(scheduler),
    totalFrames(totalMemory / frameSize), allocatedFrames(0), 
    backingStorePath("./backingStore") {

    // Initialize frame table
    initializeFrames();

    // Prepare backing store
    try {
        if (std::filesystem::exists(backingStorePath)) {
            for (const auto& entry : std::filesystem::directory_iterator(backingStorePath)) {
                std::filesystem::remove(entry.path());
            }
        }
        else {
            std::filesystem::create_directory(backingStorePath);
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error creating or cleaning backing store: " << e.what() << std::endl;
        throw;
    }
}

PagingMemoryAllocator::~PagingMemoryAllocator() {
    frameTable.clear();
    freeFrames.clear();
}

void PagingMemoryAllocator::initializeFrames() {
    for (size_t i = 0; i < totalFrames; ++i) {
        frameTable.push_back({ i, true, nullptr });
        freeFrames.push_back(i); // All frames start as free.
    }
}

void PagingMemoryAllocator::printConfiguration() const {
    std::cout << "\nPaging Memory Allocator Configuration:" << std::endl;
    std::cout << "Total Memory: " << totalMemory << " bytes" << std::endl;
    std::cout << "Frame Size: " << frameSize << " bytes" << std::endl;
    std::cout << "Total Frames: " << totalFrames << std::endl;
    std::cout << "Allocated Frames: " << allocatedFrames << std::endl;
    std::cout << "Free Frames: " << freeFrames.size() << "\n" << std::endl;
}

void PagingMemoryAllocator::visualMemory() const {
    std::cout << "Frame Allocation Map: ";
    for (const auto& frame : frameTable) {
        std::cout << (frame.isAllocatable ? "0" : "1");
    }
    std::cout << std::endl;
}

void* PagingMemoryAllocator::allocate(std::shared_ptr<AttachedProcess> process) {
    size_t requiredFrames = (process->getMemoryRequirement() + frameSize - 1) / frameSize;

    if (requiredFrames > freeFrames.size()) {
        throw std::bad_alloc();
    }

    std::vector<size_t> allocatedFrameNumbers;

    for (size_t i = 0; i < requiredFrames; ++i) {
        size_t frameNumber = freeFrames.front();
        freeFrames.pop_front();

        frameTable[frameNumber].isAllocatable = false;
        frameTable[frameNumber].process = process;

        allocatedFrameNumbers.push_back(frameNumber);
    }

    allocatedFrames += requiredFrames;

    void* processMemory = reinterpret_cast<void*>(allocatedFrameNumbers.front() * frameSize);
    process->setMemoryLocation(processMemory);

    return processMemory;
}

void PagingMemoryAllocator::deallocate(std::shared_ptr<AttachedProcess> process) {
    void* memoryLocation = process->getMemoryLocation();
    size_t startFrame = reinterpret_cast<size_t>(memoryLocation) / frameSize;

    for (auto& frame : frameTable) {
        if (frame.process == process) {
            frame.isAllocatable = true;
            frame.process.reset();
            freeFrames.push_back(frame.frameNumber);
        }
    }

    allocatedFrames -= 1; // Adjust the count based on the exact allocated frame count.
    process->setMemoryLocation(nullptr);
}

void PagingMemoryAllocator::evictOldestProcess() {
    for (auto& frame : frameTable) {
        if (!frame.isAllocatable && frame.process) {
            std::shared_ptr<AttachedProcess> process = frame.process;

            std::ofstream outFile(backingStorePath / ("process_" + std::to_string(process->getPid()) + ".txt"));

            outFile << "Process ID: " << process->getPid() << "\n";
            outFile << "Memory Requirement: " << process->getMemoryRequirement() << " bytes\n";

            deallocate(process);
            break;
        }
    }
}

int PagingMemoryAllocator::getTotalFrames() const {
    return static_cast<int>(totalFrames);
}

int PagingMemoryAllocator::getFreeFrames() const {
    return static_cast<int>(freeFrames.size());
}

std::vector<Frame> PagingMemoryAllocator::getFrameTable() const {
    return frameTable;
}
