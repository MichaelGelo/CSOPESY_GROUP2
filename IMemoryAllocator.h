#ifndef IMEMORYALLOCATOR_H
#define IMEMORYALLOCATOR_H

#include <memory>
#include <vector>

class AttachedProcess;  // Forward declaration of AttachedProcess

class IMemoryAllocator {
public:
    struct MemoryPartition {
        uint32_t slotNumber;                       // The memory partition slot number
        bool isAllocatable;                        // Is this partition allocatable (free or occupied)
        std::shared_ptr<AttachedProcess> process;  // The process that occupies this partition (if any)
    };

    virtual void* allocate(std::shared_ptr<AttachedProcess> process) = 0;
    virtual void deallocate(std::shared_ptr<AttachedProcess> process) = 0;
    virtual void visualMemory() const = 0;
    virtual void printConfiguration() const = 0;
    virtual size_t getFreeMemory() const = 0;
    virtual size_t getAllocatedSize() const = 0;
    virtual std::vector<MemoryPartition> getMemoryPartitions() const = 0;
    virtual size_t getMaximumSize() const = 0;
    virtual size_t getMinimumAllocatableSize() const = 0;
    virtual size_t getMemoryPerFrame() const = 0;
    virtual bool isAllocated(size_t index) const = 0;
    virtual MemoryPartition getPartitionAt(size_t index) const = 0;
    virtual void evictOldestProcess() = 0;
};

#endif // IMEMORYALLOCATOR_H
