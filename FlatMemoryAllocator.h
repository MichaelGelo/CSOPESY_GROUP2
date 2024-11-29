#ifndef FLATMALLOCATOR_H
#define FLATMALLOCATOR_H

#include "IMemoryAllocator.h"
#include "AttachedProcess.h"
#include <vector>
#include <unordered_map>
#include <filesystem>

class FlatMemoryAllocator : public IMemoryAllocator {
public:
    FlatMemoryAllocator(size_t maximumSize, size_t memoryPerFrame, size_t minMemoryPerProc, size_t maxMemoryPerProc);
    ~FlatMemoryAllocator();

    void* allocate(std::shared_ptr<AttachedProcess> process) override;
    void deallocate(std::shared_ptr<AttachedProcess> process) override;
    void visualMemory() const override;
    void printConfiguration() const override;
    size_t getFreeMemory() const override;
    size_t getAllocatedSize() const override;
    std::vector<IMemoryAllocator::MemoryPartition> getMemoryPartitions() const override;
    size_t getMaximumSize() const override;
    size_t getMinimumAllocatableSize() const override;
    size_t getMemoryPerFrame() const override;
    bool isAllocated(size_t index) const override;
    IMemoryAllocator::MemoryPartition getPartitionAt(size_t index) const override;
    void evictOldestProcess() override;
    size_t getPageIn()override;
    size_t getPageOut()override;

private:
    std::vector<IMemoryAllocator::MemoryPartition> memoryPartitions;
    size_t maximumSize;
    size_t allocatedSize;
    size_t memoryPerFrame;
    size_t minMemoryPerProc;
    size_t maxMemoryPerProc;
    std::vector<char> memory;
    std::vector<bool> allocationMap;
    std::unordered_map<size_t, size_t> allocatedMemoryMap;
    std::filesystem::path backingStorePath = "./backingStore";
    std::unordered_map<size_t, std::shared_ptr<AttachedProcess>> backingStore;

    void initializeMemory();
    bool canAllocateAt(size_t index, size_t size) const;
    void deallocateAt(size_t index);
};

#endif // FLATMALLOCATOR_H
