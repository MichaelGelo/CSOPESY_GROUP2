#ifndef PAGINGALLOCATOR_H
#define PAGINGALLOCATOR_H

#include "IMemoryAllocator.h"
#include "AttachedProcess.h"
#include "Frame.h"
#include <vector>
#include <unordered_map>
#include <filesystem>

#include <memory>
#include <map>
#include <string>
#include <deque>
class PagingMemoryAllocator : public IMemoryAllocator {
public:

    PagingMemoryAllocator(size_t maximumSize, size_t memoryPerFrame, size_t minMemoryPerProc, size_t maxMemoryPerProc);
    ~PagingMemoryAllocator();

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

    size_t getPageIn();
    size_t getPageOut();

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

    // paging
    size_t totalMemory;
    size_t frameSize;
    size_t totalFrames;
    size_t allocatedFrames;

    std::deque<size_t> freeFrames; // Queue to manage free frames.
    std::vector<Frame> frameTable; // Represents all frames in memory.
    void initializeFrames();


    int getTotalFrames() const;
    int getFreeFrames() const;
    std::vector<Frame> getFrameTable() const;

    std::mutex memoryMutex;
    size_t nPagedIn;
    size_t nPagedOut;

};

#endif // FLATMALLOCATOR_H
