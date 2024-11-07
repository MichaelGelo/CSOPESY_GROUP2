#ifndef FLATMALLOCATOR_H
#define FLATMALLOCATOR_H

#include "IMemoryAllocator.h" 
#include <vector>
#include <string>
#include <iostream>

class FlatMemoryAllocator : public IMemoryAllocator {
public:
    FlatMemoryAllocator(size_t maximumSize, size_t memoryPerFrame, size_t minMemoryPerProc);
    ~FlatMemoryAllocator();

    void* allocate(size_t size) override;
    void deallocate(void* ptr) override;
    std::string visualMemory() const override;
    void printConfiguration() const;
    int getFreeMemory() const;
    int getAllocatedSize() const;

private:
    size_t maximumSize;
    size_t allocatedSize;
    size_t memoryPerFrame;
    size_t minMemoryPerProc;
    std::vector<char> memory;
    std::vector<bool> allocationMap;

    void initializeMemory();
    bool canAllocateAt(size_t index, size_t size) const;
    void deallocateAt(size_t index);
};

#endif
