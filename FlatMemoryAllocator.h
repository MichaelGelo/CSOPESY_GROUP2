#ifndef FLATMALLOCATOR_H
#define FLATMALLOCATOR_H

#include "IMemoryAllocator.h" 
#include "AttachedProcess.h"
#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include <unordered_map> 

struct MemoryPartition {
    uint32_t slotNumber;
    bool isAllocatable;
    std::shared_ptr<AttachedProcess> process;
};

class FlatMemoryAllocator : public IMemoryAllocator {
public:
    FlatMemoryAllocator(size_t maximumSize, size_t memoryPerFrame, size_t minMemoryPerProc);
    ~FlatMemoryAllocator();

    void* allocate(std::shared_ptr<AttachedProcess> process) override; 
    void deallocate(std::shared_ptr<AttachedProcess> process) override;
    void visualMemory() const;
    void printConfiguration() const;
    int getFreeMemory() const;
    int getAllocatedSize() const;
    std::vector<MemoryPartition> getMemoryPartitions() const;
    int getMaximumSize() const;
    int getMinimumAllocatableSize() const;
    int getMemoryPerFrame() const;
    bool isAllocated(size_t index) const;
    MemoryPartition getPartitionAt(int index) const;
    void evictOldestProcess();

private:
    std::vector<MemoryPartition> memoryPartitions; // Ensure this is declared
    size_t maximumSize;
    size_t allocatedSize;
    size_t memoryPerFrame;
    size_t minMemoryPerProc;
    std::vector<char> memory;
    std::vector<bool> allocationMap;
    std::unordered_map<size_t, size_t> allocatedMemoryMap;
    std::filesystem::path backingStorePath = "./backingStore";
    std::unordered_map<size_t, std::shared_ptr<AttachedProcess>> backingStore;

    void initializeMemory();
    bool canAllocateAt(size_t index, size_t size) const;
    void deallocateAt(size_t index);
};

#endif
