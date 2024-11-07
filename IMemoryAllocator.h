#ifndef IMEMORYALLOCATOR_H
#define IMEMORYALLOCATOR_H

#include <cstddef>
#include <string>

class IMemoryAllocator {
public:
    virtual ~IMemoryAllocator() = default;
    virtual void* allocate(size_t size) = 0;
    virtual void deallocate(void* ptr) = 0;
    virtual std::string visualMemory() const = 0;
};

#endif
