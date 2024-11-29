#include "AllocatorPaging.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <iomanip>

AllocatorPaging::AllocatorPaging(size_t maximumSize, size_t memoryPerFrame, size_t minMemoryPerProc, size_t maxMemoryPerProc)
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



void AllocatorPaging::printConfiguration() const {
    std::cout << "\nFlat Memory Allocator Configuration:" << std::endl;
    std::cout << "Maximum Size: " << maximumSize << " bytes" << std::endl;
    std::cout << "Allocated Size: " << allocatedSize << " bytes" << std::endl;
    std::cout << "Memory per Frame: " << memoryPerFrame << " bytes" << std::endl;
    std::cout << "Minimum Memory per Process: " << minMemoryPerProc << " bytes\n" << std::endl;
}

AllocatorPaging::~AllocatorPaging() {
    frameTable.clear();
    freeFrames.clear();
}

//frames data still not gotten here
// same lang ung frame table sa freeequeue ata
void AllocatorPaging::initializeFrames() {
    for (size_t i = 0; i < totalFrames; ++i) {
        // Use Frame constructor from frame.h
        frameTable.push_back(Frame(i, frameSize, true));
        freeQueue.push_back(i); // All frames start as free.
    }
}

void AllocatorPaging::visualMemory() const {
    std::cout << "Allocation Map: ";
    for (bool occupied : allocationMap) {
        std::cout << (occupied ? "1" : "0");
    }
    std::cout << std::endl;
}


//wala pa si backing store, copypaste backing store in flatmemory
void AllocatorPaging::evictOldestProcess() {
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

void* AllocatorPaging::allocate(std::shared_ptr<AttachedProcess> process) {
    // Change this to just use P of the process
    size_t requiredFrames = (process->getMemoryRequirement() + memoryPerFrame - 1) / memoryPerFrame; //should just be page
    // Check if enough free frames are available
    if (requiredFrames > freeQueue.size()) {
        std::cerr << "Allocation failed: Not enough free frames" << std::endl;
        throw std::bad_alloc(); // Not enough free frames
    }

    // Get the first available frame index from freeQueue
    size_t frameIndex = freeQueue.back();

    // Allocate frames for the process
    for (size_t i = 0; i < requiredFrames; ++i) {
        // Remove frame from freeQueue
        freeQueue.pop_back();

        // Update frame table
        frameTable[frameIndex + i].setIsAllocatable(false);

        // Create a new Page for the process
        std::string pageName = "Page_" + std::to_string(process->getPid()) + "_" + std::to_string(i);
        auto page = std::make_shared<Page>(pageName, process->getPid(), memoryPerFrame);
        frameTable[frameIndex + i].setCurrentPage(page);
    }

    // Update allocation counters
    allocatedFrames += requiredFrames;

    // Calculate memory location and set for the process
    void* processMemory = reinterpret_cast<void*>(frameIndex * memoryPerFrame);
    process->setMemoryLocation(processMemory);

    nPagedIn++;
    return processMemory;
}

// waka pa ung magic
void AllocatorPaging::deallocate(std::shared_ptr<AttachedProcess> process) {
    void* memoryLocation = process->getMemoryLocation();
    if (memoryLocation == nullptr) {
        std::cerr << "Deallocate failed: No memory to deallocate" << std::endl;
        return; // No memory to deallocate
    }

    // Calculate frame index and number of frames
    size_t frameIndex = reinterpret_cast<size_t>(memoryLocation) / memoryPerFrame;
    size_t requiredFrames = (process->getMemoryRequirement() + memoryPerFrame - 1) / memoryPerFrame;

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


    // Clear process memory location
    process->setMemoryLocation(nullptr);

    nPagedOut++;
}

void AllocatorPaging::initializeMemory() {
    std::fill(allocationMap.begin(), allocationMap.end(), false);
    memoryPartitions.clear();
    for (size_t i = 0; i < maximumSize; i += memoryPerFrame) {
        memoryPartitions.push_back({ static_cast<uint32_t>(i / memoryPerFrame), true, nullptr });
    }
}

//idk if used
bool AllocatorPaging::canAllocateAt(size_t index, size_t size) const {
    if (index % memoryPerFrame != 0 || index + size > allocationMap.size()) return false;
    for (size_t i = index; i < index + size; ++i) {
        if (allocationMap[i]) return false;
    }
    return true;
}

void AllocatorPaging::deallocateAt(size_t index) {
    size_t size = 0;
    while (index + size < allocationMap.size() && allocationMap[index + size]) {
        allocationMap[index + size] = false;
        ++size;
    }
    allocatedSize -= size;
}

size_t AllocatorPaging::getAllocatedSize() const {
    return allocatedSize;
}

size_t AllocatorPaging::getFreeMemory() const {
    return static_cast<int>(maximumSize - allocatedSize);
}

std::vector<IMemoryAllocator::MemoryPartition> AllocatorPaging::getMemoryPartitions() const {
    return memoryPartitions;
}

size_t AllocatorPaging::getMaximumSize() const {
    return maximumSize;
}

size_t AllocatorPaging::getMemoryPerFrame() const {
    return memoryPerFrame;
}

IMemoryAllocator::MemoryPartition AllocatorPaging::getPartitionAt(size_t index) const {
    if (index >= 0 && index < static_cast<int>(memoryPartitions.size())) {
        return memoryPartitions[index];
    }
    return IMemoryAllocator::MemoryPartition{};
}

size_t AllocatorPaging::getMinimumAllocatableSize() const {
    return static_cast<int>(minMemoryPerProc);
}

bool AllocatorPaging::isAllocated(size_t index) const {
    if (index >= allocationMap.size()) {
        throw std::out_of_range("Index out of range");
    }
    return allocationMap[index];
}

int AllocatorPaging::getTotalFrames() const {
    return static_cast<int>(totalFrames);
}

int AllocatorPaging::getFreeFrames() const {
    return static_cast<int>(freeFrames.size());
}

std::vector<Frame> AllocatorPaging::getFrameTable() const {
    return frameTable;
}

size_t AllocatorPaging::getPageIn() {
    std::lock_guard<std::mutex> lock(memoryMutex);
    return nPagedIn;
}
size_t AllocatorPaging::getPageOut() {
    std::lock_guard<std::mutex> lock(memoryMutex);
    return nPagedOut;
}

size_t AllocatorPaging::allocateFrames(size_t numFrame, std::shared_ptr<Process> process) {
    std::lock_guard<std::mutex> lock(frameQueueMutex);  // Lock the mutex for thread safety

    // Check if there are enough frames in the queue
    if (frameQueue.size() < numFrame) {
        std::cerr << "Not enough frames available!" << std::endl;
        return -1;  // Return an invalid index if not enough frames are available
    }

    size_t frameIndex = frameQueue.front()->getFrameNum();  // Get the first available frame number
    for (size_t i = 0; i < numFrame; ++i) {
        frameQueue.pop();  // Remove the frame from the queue (allocate it)
        nPagedIn++;  // Increment the number of pages brought into memory
    }
    return frameIndex;  // Return the starting frame index
}



void AllocatorPaging::deallocateFrames(size_t numFrame, size_t frameIndex) {
    std::lock_guard<std::mutex> lock(frameQueueMutex);  // Lock the mutex for thread safety

    // Push frames back into the frameQueue to make them available again
    for (size_t i = 0; i < numFrame; ++i) {
        auto frame = std::make_shared<Frame>(frameIndex + i, memoryPerFrame, false);  // Create a new frame
        frameQueue.push(frame);  // Push the frame back to the queue (deallocate it)
        nPagedOut++;  // Increment the number of pages paged out
    }
}

