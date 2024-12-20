#include "FlatMemoryAllocator.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <iomanip>

FlatMemoryAllocator::FlatMemoryAllocator(size_t maximumSize, size_t memoryPerFrame, size_t minMemoryPerProc, size_t maxMemoryPerProc)
    : maximumSize(maximumSize),
    allocatedSize(0),
    memoryPerFrame(memoryPerFrame),
    minMemoryPerProc(minMemoryPerProc),
    maxMemoryPerProc(maxMemoryPerProc),
    allocationMap(maximumSize, false),
    backingStorePath("./backingStore") {
    memory.resize(maximumSize);
    initializeMemory();

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

void FlatMemoryAllocator::evictOldestProcess() {
    if (memoryPartitions.size() == 1 && maximumSize == memoryPerFrame) {
        auto& partition = memoryPartitions[0];

        if (!partition.isAllocatable && partition.process) {
            auto process = partition.process;
            if (!process) {
                std::cerr << "Error: Process is null during eviction.\n";
                return;
            }

            auto now = std::chrono::system_clock::now();
            auto time_t_now = std::chrono::system_clock::to_time_t(now);
            std::tm tm_now;

            if (localtime_s(&tm_now, &time_t_now) != 0) {
                std::cerr << "Error: Failed to get local time for eviction timestamp.\n";
                return;
            }

            std::ostringstream timestampStream;
            timestampStream << std::put_time(&tm_now, "%Y-%m-%d %H:%M:%S");

            // Write process details to backing store
            std::string fileName = "process_" + std::to_string(process->getPid()) + ".txt";
            std::filesystem::path filePath = backingStorePath / fileName;

            try {
                std::ofstream outFile(filePath);
                outFile << "Process ID: " << process->getPid() << std::endl;
                outFile << "Memory Requirement: " << process->getMemoryRequirement() << " bytes" << std::endl;
                outFile << "Eviction Timestamp: " << timestampStream.str() << std::endl;
                outFile.close();
            }
            catch (const std::exception& e) {
                std::cerr << "Error writing to backing store file: " << e.what() << std::endl;
            }

            backingStore[process->getPid()] = process;

            deallocate(process);
        }
    }
    else {
        for (auto& partition : memoryPartitions) {
            if (!partition.isAllocatable && partition.process) {
                auto process = partition.process;
                if (!process) {
                    std::cerr << "Error: Process is null during eviction.\n";
                    continue;
                }

                auto now = std::chrono::system_clock::now();
                auto time_t_now = std::chrono::system_clock::to_time_t(now);
                std::tm tm_now;

                if (localtime_s(&tm_now, &time_t_now) != 0) {
                    std::cerr << "Error: Failed to get local time for eviction timestamp.\n";
                    return;
                }

                std::ostringstream timestampStream;
                timestampStream << std::put_time(&tm_now, "%Y-%m-%d %H:%M:%S");

                std::string fileName = "process_" + std::to_string(process->getPid()) + ".txt";
                std::filesystem::path filePath = backingStorePath / fileName;

                try {
                    std::ofstream outFile(filePath);
                    outFile << "Process ID: " << process->getPid() << std::endl;
                    outFile << "Memory Requirement: " << process->getMemoryRequirement() << " bytes" << std::endl;
                    outFile << "Eviction Timestamp: " << timestampStream.str() << std::endl;
                    outFile.close();
                }
                catch (const std::exception& e) {
                    std::cerr << "Error writing to backing store file: " << e.what() << std::endl;
                }

                // Store the evicted process in the backing store
                backingStore[process->getPid()] = process;

                // Deallocate the memory used by the process
                deallocate(process);
                break;
            }
        }
    }
}


void* FlatMemoryAllocator::allocate(std::shared_ptr<AttachedProcess> process) {
    size_t size = process->getMemoryRequirement();

    if (size > memoryPerFrame || size < minMemoryPerProc) {
        throw std::bad_alloc();
    }

    if (maximumSize == memoryPerFrame) {
        if (!allocationMap[0]) { 
            allocationMap[0] = true;
            allocatedSize = size;

            void* allocatedMemory = memory.data();
            process->setMemoryLocation(allocatedMemory);

            allocatedMemoryMap[reinterpret_cast<size_t>(allocatedMemory)] = size;
            memoryPartitions[0] = { 0, false, process };

            return allocatedMemory;
        }
        else {
            evictOldestProcess();
        }
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
        size_t startIndex = reinterpret_cast<char*>(process->getMemoryLocation()) - memory.data();\

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
    if (maximumSize == memoryPerFrame) {
        return !allocationMap[0]; 
    }

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

size_t FlatMemoryAllocator::getAllocatedSize() const {
    return allocatedSize;
}

size_t FlatMemoryAllocator::getFreeMemory() const {
    return static_cast<int>(maximumSize - allocatedSize);
}

std::vector<IMemoryAllocator::MemoryPartition> FlatMemoryAllocator::getMemoryPartitions() const {
    return memoryPartitions;
}

size_t FlatMemoryAllocator::getMaximumSize() const {
    return maximumSize;
}

size_t FlatMemoryAllocator::getMemoryPerFrame() const {
    return memoryPerFrame;
}

IMemoryAllocator::MemoryPartition FlatMemoryAllocator::getPartitionAt(size_t index) const {
    if (index >= 0 && index < static_cast<int>(memoryPartitions.size())) {
        return memoryPartitions[index];
    }
    return IMemoryAllocator::MemoryPartition{};
}

size_t FlatMemoryAllocator::getMinimumAllocatableSize() const {
    return static_cast<int>(minMemoryPerProc);
}

bool FlatMemoryAllocator::isAllocated(size_t index) const {
    if (index >= allocationMap.size()) {
        throw std::out_of_range("Index out of range");
    }
    return allocationMap[index];
}

size_t FlatMemoryAllocator::getPageIn() {
    return 0;
}
size_t FlatMemoryAllocator::getPageOut() {
    return 0;
}