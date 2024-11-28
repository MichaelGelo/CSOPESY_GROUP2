#include "PagingMemoryAllocator.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <iomanip>

PagingMemoryAllocator::PagingMemoryAllocator(size_t maximumSize, size_t memoryPerFrame, size_t minMemoryPerProc, size_t maxMemoryPerProc)
    : maximumSize(maximumSize),
    allocatedSize(0),
    memoryPerFrame(memoryPerFrame),
    minMemoryPerProc(minMemoryPerProc),
    maxMemoryPerProc(maxMemoryPerProc),
    allocationMap(maximumSize, false),
    backingStorePath("./backingStore") {
    memory.resize(maximumSize);

    initializeFrames();
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
        std::cout << "Error creating or cleaning backing store directory: " << e.what() << std::endl;
        throw;
    }
}



void PagingMemoryAllocator::printConfiguration() const {
    std::cout << "\nFlat Memory Allocator Configuration:" << std::endl;
    std::cout << "Maximum Size: " << maximumSize << " bytes" << std::endl;
    std::cout << "Allocated Size: " << allocatedSize << " bytes" << std::endl;
    std::cout << "Memory per Frame: " << memoryPerFrame << " bytes" << std::endl;
    std::cout << "Minimum Memory per Process: " << minMemoryPerProc << " bytes\n" << std::endl;
}

PagingMemoryAllocator::~PagingMemoryAllocator() {
    frameTable.clear();
    freeFrames.clear();
}

void PagingMemoryAllocator::initializeFrames() {
    for (size_t i = 0; i < totalFrames; ++i) {
        // Use Frame constructor from frame.h
        frameTable.push_back(Frame(i, frameSize, true));
        freeFrames.push_back(i); // All frames start as free.
    }
}

void PagingMemoryAllocator::visualMemory() const {
    std::cout << "Allocation Map: ";
    for (bool occupied : allocationMap) {
        std::cout << (occupied ? "1" : "0");
    }
    std::cout << std::endl;
}

void PagingMemoryAllocator::evictOldestProcess() {
    for (auto& frame : frameTable) {
        if (!frame.isAllocatable() && frame.getCurrentPage()) {
            std::shared_ptr<Page> page = frame.getCurrentPage();
            
            std::ofstream outFile(backingStorePath / ("process_" + std::to_string(page->getPid()) + ".txt"));

            outFile << "Process ID: " << page->getPid() << "\n";
            outFile << "Page Name: " << page->getName() << "\n";
            
            // You'll need to find the process with this PID and deallocate
            // This might require additional logic depending on how you track processes
            // For example:
            // std::shared_ptr<AttachedProcess> process = findProcessByPid(page->getPid());
            // if (process) {
            //     deallocate(process);
            //     break;
            // }
        }
    }
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
        frameTable[frameIndex + i].setIsAllocatable(false);

        // Create a new Page for the process
        // Use process name or generate a unique page name
        std::string pageName = "Page_" + std::to_string(process->getPid()) + "_" + std::to_string(i);
        auto page = std::make_shared<Page>(pageName, process->getPid(), frameSize);
        frameTable[frameIndex + i].setCurrentPage(page);
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
        // Clear page from frame
        frameTable[frameIndex + i].setIsAllocatable(true);
        frameTable[frameIndex + i].setCurrentPage(nullptr);

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

void PagingMemoryAllocator::initializeMemory() {
    std::fill(allocationMap.begin(), allocationMap.end(), false);
    memoryPartitions.clear();
    for (size_t i = 0; i < maximumSize; i += memoryPerFrame) {
        memoryPartitions.push_back({ static_cast<uint32_t>(i / memoryPerFrame), true, nullptr });
    }
}

bool PagingMemoryAllocator::canAllocateAt(size_t index, size_t size) const {
    if (index % memoryPerFrame != 0 || index + size > allocationMap.size()) return false;
    for (size_t i = index; i < index + size; ++i) {
        if (allocationMap[i]) return false;
    }
    return true;
}

void PagingMemoryAllocator::deallocateAt(size_t index) {
    size_t size = 0;
    while (index + size < allocationMap.size() && allocationMap[index + size]) {
        allocationMap[index + size] = false;
        ++size;
    }
    allocatedSize -= size;
}

size_t PagingMemoryAllocator::getAllocatedSize() const {
    return allocatedSize;
}

size_t PagingMemoryAllocator::getFreeMemory() const {
    return static_cast<int>(maximumSize - allocatedSize);
}

std::vector<IMemoryAllocator::MemoryPartition> PagingMemoryAllocator::getMemoryPartitions() const {
    return memoryPartitions;
}

size_t PagingMemoryAllocator::getMaximumSize() const {
    return maximumSize;
}

size_t PagingMemoryAllocator::getMemoryPerFrame() const {
    return memoryPerFrame;
}

IMemoryAllocator::MemoryPartition PagingMemoryAllocator::getPartitionAt(size_t index) const {
    if (index >= 0 && index < static_cast<int>(memoryPartitions.size())) {
        return memoryPartitions[index];
    }
    return IMemoryAllocator::MemoryPartition{};
}

size_t PagingMemoryAllocator::getMinimumAllocatableSize() const {
    return static_cast<int>(minMemoryPerProc);
}

bool PagingMemoryAllocator::isAllocated(size_t index) const {
    if (index >= allocationMap.size()) {
        throw std::out_of_range("Index out of range");
    }
    return allocationMap[index];
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