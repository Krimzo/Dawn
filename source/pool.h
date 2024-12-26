#pragma once

#include "memory.h"
#include "node.h"
#include "stack.h"


namespace dawn
{
struct MemoryPools
{
    MemoryPool<ScopeObject, 256> scope_memory;
    MemoryPool<Value, 1024> value_memory;
    MemoryPool<Node, 1024> node_memory;

    ~MemoryPools() noexcept;
};

MemoryPool<ScopeObject, 256>& scope_pool();
MemoryPool<Value, 1024>& value_pool();
MemoryPool<Node, 1024>& node_pool();
}
