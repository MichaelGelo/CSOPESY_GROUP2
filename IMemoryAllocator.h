#ifndef IMEMORYALLOCATOR_H
#define IMEMORYALLOCATOR_H

#include "AttachedProcess.h"
#include <cstddef>
#include <memory>
#include <string>

class IMemoryAllocator {
public:
    virtual ~IMemoryAllocator() = default;
    virtual void* allocate(std::shared_ptr<AttachedProcess> process) = 0;
    virtual void deallocate(std::shared_ptr<AttachedProcess> process) = 0;
};

#endif
