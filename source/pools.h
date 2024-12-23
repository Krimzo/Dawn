#pragma once

#include "memory.h"
#include "values.h"
#include "stack.h"


namespace dawn
{
struct MemoryPools
{
    MemoryPool<ScopeObject, 256> scope_memory;
    MemoryPool<Value, 1024> value_memory;

    ~MemoryPools() noexcept;
};

MemoryPools& memory_pools();
}
