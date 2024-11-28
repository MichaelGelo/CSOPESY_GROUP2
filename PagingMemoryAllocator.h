#ifndef PAGINGMEMORYALLOCATOR_H
#define PAGINGMEMORYALLOCATOR_H

#include <vector>
#include <memory>
#include <map>
#include <string>
#include <filesystem>
#include <deque>
#include "AttachedProcess.h" // Assume this is a class representing processes.
#include <mutex>
class Scheduler;

struct Frame {
    size_t frameNumber;
    bool isAllocatable;
    std::shared_ptr<AttachedProcess> process;
};

class PagingMemoryAllocator {
public:
    PagingMemoryAllocator(size_t totalMemory, size_t frameSize, Scheduler& scheduler);
    ~PagingMemoryAllocator();

    void* allocate(std::shared_ptr<AttachedProcess> process);
    void deallocate(std::shared_ptr<AttachedProcess> process);
    void printConfiguration() const;
    void visualMemory() const;
    int getTotalFrames() const;
    int getFreeFrames() const;
    std::vector<Frame> getFrameTable() const;

    size_t getPageIn();  // Remove const
    size_t getPageOut(); // Remove const

private:
    size_t totalMemory;
    size_t frameSize;
    size_t totalFrames;
    size_t allocatedFrames;

    std::deque<size_t> freeFrames; // Queue to manage free frames.
    std::vector<Frame> frameTable; // Represents all frames in memory.
    std::map<size_t, std::shared_ptr<AttachedProcess>> backingStore;

    std::filesystem::path backingStorePath;

    Scheduler& scheduler;

    void initializeFrames();
    void evictOldestProcess();

    mutable std::mutex memoryMutex;  // Make mutex mutable
    std::atomic<size_t> nPagedIn{ 0 }; // Use atomic for thread-safety
    std::atomic<size_t> nPagedOut{ 0 }; // Use atomic for thread-safety
};

#endif
