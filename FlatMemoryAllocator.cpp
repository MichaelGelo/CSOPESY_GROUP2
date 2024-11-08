#include "FlatMemoryAllocator.h"
#include <iostream>
#include <stdexcept>
#include <sstream>

FlatMemoryAllocator::FlatMemoryAllocator(size_t maximumSize, size_t memoryPerFrame, size_t minMemoryPerProc)
    : maximumSize(maximumSize), allocatedSize(0), memoryPerFrame(memoryPerFrame), minMemoryPerProc(minMemoryPerProc), allocationMap(maximumSize, false) {
    memory.resize(maximumSize);
    initializeMemory();
}

void FlatMemoryAllocator::printConfiguration() const {
    std::cout << "\nFlat Memory Allocator Configuration:" << std::endl;
    std::cout << "Maximum Size: " << maximumSize << " bytes" << std::endl;
    std::cout << "Allocated Size: " << allocatedSize << " bytes" << std::endl;
    std::cout << "Memory per Frame: " << memoryPerFrame << " bytes" << std::endl;
    std::cout << "Minimum Memory per Process: " << minMemoryPerProc << " bytes\n" << std::endl;
}

FlatMemoryAllocator::~FlatMemoryAllocator() {
    memory.clear();
}

void* FlatMemoryAllocator::allocate(std::shared_ptr<AttachedProcess> process) {
    size_t size = process->getMemoryRequirement();

    // Ensure the memory requirement is valid and there’s enough available space
    if (size == 0 || allocatedSize + size > maximumSize) {
        throw std::bad_alloc();
    }

    // Search for a suitable free block in the memory
    for (size_t i = 0; i <= maximumSize - size; ++i) {
        if (canAllocateAt(i, size)) {
            // Mark this block as allocated in the allocationMap
            std::fill(allocationMap.begin() + i, allocationMap.begin() + i + size, true);
            allocatedSize += size;

            // Assign the memory address to the process and log it
            void* allocatedMemory = &memory[i];
            process->setMemoryLocation(allocatedMemory);

            std::cout << "Allocated process " << process->getPid()
                << " at unique location: " << allocatedMemory
                << " (starting at index " << i << "), for " << size << " bytes." << std::endl;

            visualMemory(); // REMOVE IF NOT DEBUGGING
            return allocatedMemory;
        }
    }

    // If no suitable block is found, throw an error
    throw std::bad_alloc();
}

void FlatMemoryAllocator::visualMemory() const {
    std::cout << "Allocation Map: ";
    for (bool occupied : allocationMap) {
        std::cout << (occupied ? "1" : "0");
    }
    std::cout << std::endl;
}

void FlatMemoryAllocator::deallocate(std::shared_ptr<AttachedProcess> process) {
    void* ptr = process->getMemoryLocation();
    size_t index = static_cast<char*>(ptr) - memory.data();

    if (index < maximumSize && allocationMap[index]) {
        deallocateAt(index);
        process->setMemoryLocation(nullptr);  // Clear the memory location in the process
    }
    else {
        throw std::invalid_argument("Pointer not allocated by this allocator.");
    }
}

void FlatMemoryAllocator::initializeMemory() {
    std::fill(allocationMap.begin(), allocationMap.end(), false);
}

bool FlatMemoryAllocator::canAllocateAt(size_t index, size_t size) const {
    // Ensure block is within bounds and not occupied
    if (index + size > allocationMap.size()) return false;
    for (size_t i = index; i < index + size; ++i) {
        if (allocationMap[i]) return false;  
    }
    return true;  
}

void FlatMemoryAllocator::deallocateAt(size_t index) {
    size_t size = 0;
    while (index + size < allocationMap.size() && allocationMap[index + size]) {
        allocationMap[index + size] = false;
        ++size;
    }
    allocatedSize -= size; 
}

int FlatMemoryAllocator::getAllocatedSize() const {
    return static_cast<int>(allocatedSize);
}

int FlatMemoryAllocator::getFreeMemory() const {
    return static_cast<int>(maximumSize - allocatedSize);
}

std::vector<MemoryPartition> FlatMemoryAllocator::getMemoryPartitions() const {
    return memoryPartitions;
}

int FlatMemoryAllocator::getMaximumSize() const {
    return static_cast<int>(maximumSize);
}

int FlatMemoryAllocator::getMemoryPerFrame() const {
    return static_cast<int>(memoryPerFrame);
}

MemoryPartition FlatMemoryAllocator::getPartitionAt(int index) const {
    if (index >= 0 && index < memoryPartitions.size()) {
        return memoryPartitions[index];
    }
    return MemoryPartition{};  
}

int FlatMemoryAllocator::getMinimumAllocatableSize() const {
    return static_cast<int>(minMemoryPerProc);
}

bool FlatMemoryAllocator::isAllocated(size_t index) const {
    if (index < 0 || index >= allocationMap.size()) {
        throw std::out_of_range("Index out of range");
    }
    return allocationMap[index]; 
}