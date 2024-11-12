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

void FlatMemoryAllocator::visualMemory() const {
    std::cout << "Allocation Map: ";
    for (bool occupied : allocationMap) {
        std::cout << (occupied ? "1" : "0");
    }
    std::cout << std::endl;
}

void* FlatMemoryAllocator::allocate(std::shared_ptr<AttachedProcess> process) {
    size_t size = process->getMemoryRequirement();

    if (size < minMemoryPerProc || size == 0 || allocatedSize + size > maximumSize) {
        throw std::bad_alloc();
    }

    for (size_t i = 0; i <= maximumSize - size; i += memoryPerFrame) {
        if (canAllocateAt(i, size)) {
            std::fill(allocationMap.begin() + i, allocationMap.begin() + i + size, true);
            allocatedSize += size;

            void* allocatedMemory = &memory[i];
            process->setMemoryLocation(allocatedMemory);

            allocatedMemoryMap[reinterpret_cast<size_t>(allocatedMemory)] = size;
            memoryPartitions.push_back({ static_cast<uint32_t>(i / memoryPerFrame), false, process });

            return allocatedMemory;
        }
    }
    throw std::bad_alloc();
}

void FlatMemoryAllocator::deallocate(std::shared_ptr<AttachedProcess> process) {
    size_t memoryAddress = reinterpret_cast<size_t>(process->getMemoryLocation());
    auto it = allocatedMemoryMap.find(memoryAddress);

    if (it != allocatedMemoryMap.end()) {
        size_t size = it->second;
        size_t startIndex = reinterpret_cast<char*>(process->getMemoryLocation()) - memory.data();  // Cast to char*

        std::fill(allocationMap.begin() + startIndex, allocationMap.begin() + startIndex + size, false);
        allocatedSize -= size;

        allocatedMemoryMap.erase(it);
        process->setMemoryLocation(nullptr);

        for (auto& partition : memoryPartitions) {
            if (partition.slotNumber == startIndex / memoryPerFrame && partition.process == process) {
                partition.isAllocatable = true;
                partition.process = nullptr;
                break;
            }
        }
    }
}

void FlatMemoryAllocator::initializeMemory() {
    std::fill(allocationMap.begin(), allocationMap.end(), false);
    memoryPartitions.clear();
    for (size_t i = 0; i < maximumSize; i += memoryPerFrame) {
        memoryPartitions.push_back({ static_cast<uint32_t>(i / memoryPerFrame), true, nullptr });
    }
}

bool FlatMemoryAllocator::canAllocateAt(size_t index, size_t size) const {
    if (index % memoryPerFrame != 0 || index + size > allocationMap.size()) return false;
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
    if (index >= 0 && index < static_cast<int>(memoryPartitions.size())) {
        return memoryPartitions[index];
    }
    return MemoryPartition{};
}

int FlatMemoryAllocator::getMinimumAllocatableSize() const {
    return static_cast<int>(minMemoryPerProc);
}

bool FlatMemoryAllocator::isAllocated(size_t index) const {
    if (index >= allocationMap.size()) {
        throw std::out_of_range("Index out of range");
    }
    return allocationMap[index];
}
