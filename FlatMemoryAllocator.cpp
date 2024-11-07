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
    std::cout << "\n" << std::endl;

    std::cout << "Flat Memory Allocator Configuration:" << std::endl;
    std::cout << "Maximum Size: " << maximumSize << " bytes" << std::endl;
    std::cout << "Allocated Size: " << allocatedSize << " bytes" << std::endl;
    std::cout << "Memory per Frame: " << memoryPerFrame << " bytes" << std::endl;
    std::cout << "Minimum Memory per Process: " << minMemoryPerProc << " bytes" << std::endl;

    std::cout << "\n" << std::endl;

}
 
// NOT SURE PA SA CODES HERE

FlatMemoryAllocator::~FlatMemoryAllocator() {
    memory.clear();
}

void* FlatMemoryAllocator::allocate(size_t size) {
    if (size == 0 || allocatedSize + size > maximumSize) {
        throw std::bad_alloc(); // Handle allocation failure
    }

    for (size_t i = 0; i <= maximumSize - size; ++i) {
        if (canAllocateAt(i, size)) {
            // Mark this memory as allocated
            std::fill(allocationMap.begin() + i, allocationMap.begin() + i + size, true);
            allocatedSize += size;
            return &memory[i]; // Return pointer to allocated memory
        }
    }
    throw std::bad_alloc(); // No suitable block found
}

void FlatMemoryAllocator::deallocate(void* ptr) {
    size_t index = static_cast<char*>(ptr) - memory.data();
    if (index < maximumSize && allocationMap[index]) {
        // Deallocate the block starting from this index
        deallocateAt(index);
    }
    else {
        throw std::invalid_argument("Pointer not allocated by this allocator.");
    }
}

std::string FlatMemoryAllocator::visualMemory() const {
    std::ostringstream oss;
    for (size_t i = 0; i < maximumSize; ++i) {
        oss << (allocationMap[i] ? "1" : "0"); // Allocated or Free
    }
    return oss.str();
}

void FlatMemoryAllocator::initializeMemory() {
    std::fill(allocationMap.begin(), allocationMap.end(), false);
}

bool FlatMemoryAllocator::canAllocateAt(size_t index, size_t size) const {
    for (size_t i = index; i < index + size; ++i) {
        if (i >= allocationMap.size() || allocationMap[i]) {
            return false; // Block is already allocated or out of bounds
        }
    }
    return true; // Block is free
}

void FlatMemoryAllocator::deallocateAt(size_t index) {
    // Assuming a fixed size allocation
    size_t size = 0;
    while (index + size < allocationMap.size() && allocationMap[index + size]) {
        allocationMap[index + size] = false; // Mark as free
        ++size;
    }
    allocatedSize -= size; // Update allocated size
}

int FlatMemoryAllocator::getAllocatedSize() const {
    return allocatedSize; 
}

int FlatMemoryAllocator::getFreeMemory() const {
    return maximumSize - allocatedSize;
}
