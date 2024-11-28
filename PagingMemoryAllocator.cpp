#include "PagingMemoryAllocator.h"
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <mutex>

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

size_t PagingMemoryAllocator::getPageIn() {
    std::cerr << "Getting PageIn: " << nPagedIn << std::endl;
    return nPagedIn;
}

size_t PagingMemoryAllocator::getPageOut() {
    std::cerr << "Getting PageOut: " << nPagedOut << std::endl;
    return nPagedOut;
}

void* PagingMemoryAllocator::allocate(std::shared_ptr<AttachedProcess> process) {
    // Calculate number of frames needed
    size_t requiredFrames = (process->getMemoryRequirement() + frameSize - 1) / frameSize;

    // Check if enough free frames are available
    if (requiredFrames > freeFrames.size()) {
        std::cerr << "Allocation failed: Not enough free frames" << std::endl;
        throw std::bad_alloc(); // Not enough free frames
    }

    // Get the first available frame index
    size_t frameIndex = freeFrames.back();

    // Allocate frames for the process
    for (size_t i = 0; i < requiredFrames; ++i) {
        // Remove frame from free list
        freeFrames.pop_back();

        // Update frame table
        frameTable[frameIndex + i].isAllocatable = false;
        frameTable[frameIndex + i].process = process;
    }

    // Update allocation counters
    allocatedFrames += requiredFrames;

    // Increment page-in counter
    nPagedIn += requiredFrames;
    std::cerr << "Allocated " << requiredFrames << " frames. Total PagedIn: " << nPagedIn << std::endl;

    // Calculate memory location and set for the process
    void* processMemory = reinterpret_cast<void*>(frameIndex * frameSize);
    process->setMemoryLocation(processMemory);

    return processMemory;
}

void PagingMemoryAllocator::deallocate(std::shared_ptr<AttachedProcess> process) {
    void* memoryLocation = process->getMemoryLocation();
    if (memoryLocation == nullptr) {
        std::cerr << "Deallocate failed: No memory to deallocate" << std::endl;
        return; // No memory to deallocate
    }

    // Calculate frame index and number of frames
    size_t frameIndex = reinterpret_cast<size_t>(memoryLocation) / frameSize;
    size_t requiredFrames = (process->getMemoryRequirement() + frameSize - 1) / frameSize;

    // Remove frames from frame map and mark as free
    for (size_t i = 0; i < requiredFrames; ++i) {
        // Clear process from frame
        frameTable[frameIndex + i].isAllocatable = true;
        frameTable[frameIndex + i].process.reset();

        // Add frame back to free list
        freeFrames.push_back(frameIndex + i);
    }

    // Update allocation counters
    allocatedFrames -= requiredFrames;

    // Increment page-out counter
    nPagedOut += requiredFrames;
    std::cerr << "Deallocated " << requiredFrames << " frames. Total PagedOut: " << nPagedOut << std::endl;

    // Clear process memory location
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
