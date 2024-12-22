#include "pools.h"


static dawn::MemoryPools pools;

dawn::MemoryPools::~MemoryPools() noexcept
{
    for ( auto& chunk : value_memory.m_chunks )
    {
        for ( auto& regist : chunk.m_space )
        {
            auto& value = regist.value;
            value.reset();
        }
    }
    for ( auto& chunk : scope_memory.m_chunks )
    {
        for ( auto& regist : chunk.m_space )
        {
            auto& value = regist.value;
            value.objects.clear();
            value.parent = {};
        }
    }
}

dawn::MemoryPools& dawn::memory_pools()
{
    return pools;
}
