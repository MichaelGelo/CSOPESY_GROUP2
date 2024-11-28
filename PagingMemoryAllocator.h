#ifndef PAGINGMEMORYALLOCATOR_H
#define PAGINGMEMORYALLOCATOR_H
#include <vector>
#include <memory>
#include <map>
#include <string>
#include <filesystem>
#include <deque>
#include "AttachedProcess.h"
#include "Frame.h" // Include the existing Frame header
#include <mutex>

class Scheduler;

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
    size_t getPageIn();
    size_t getPageOut();
private:
    size_t totalMemory;
    size_t frameSize;
    size_t totalFrames;
    size_t allocatedFrames;
    std::deque<size_t> freeFrames;
    std::vector<Frame> frameTable;
    std::map<size_t, std::shared_ptr<AttachedProcess>> backingStore;
    std::filesystem::path backingStorePath;
    Scheduler& scheduler;
    void initializeFrames();
    void evictOldestProcess();
    mutable std::mutex memoryMutex;
    std::atomic<size_t> nPagedIn{ 0 };
    std::atomic<size_t> nPagedOut{ 0 };
};
#endif