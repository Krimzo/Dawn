#include "pool.h"


static dawn::MemoryPools pools;

dawn::MemoryPools::~MemoryPools() noexcept
{
    for ( auto& chunk : node_memory.m_chunks )
    {
        for ( auto& regist : chunk.m_space )
            regist.value.reset();

    }
    for ( auto& chunk : value_memory.m_chunks )
    {
        for ( auto& regist : chunk.m_space )
            regist.value.reset();

    }
    for ( auto& chunk : scope_memory.m_chunks )
    {
        for ( auto& regist : chunk.m_space )
            regist.value.reset( {} );
    }
}

dawn::MemoryPool<dawn::ScopeObject, 256>& dawn::scope_pool()
{
    return pools.scope_memory;
}

dawn::MemoryPool<dawn::Value, 1024>& dawn::value_pool()
{
    return pools.value_memory;
}

dawn::MemoryPool<dawn::Node, 1024>& dawn::node_pool()
{
    return pools.node_memory;
}
