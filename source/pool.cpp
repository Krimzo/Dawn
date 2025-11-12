#include "pool.h"


static
#ifndef DAWN_NO_THREADS
thread_local
#endif
dawn::MemoryPools pools;

dawn::MemoryPools::~MemoryPools() noexcept
{
    static constexpr auto clear_chunks = []<typename T, Int N>( MemoryPool<T, N>&pool )
    {
        for ( auto& chunk : pool.m_chunks )
        {
            for ( auto& regist : chunk.m_space )
                regist.value() = T{};
        }
    };

    clear_chunks( node_memory );
    clear_chunks( frame_memory );
    clear_chunks( bool_memory );
    clear_chunks( int_memory );
    clear_chunks( float_memory );
    clear_chunks( char_memory );
    clear_chunks( string_memory );
    clear_chunks( function_memory );
    clear_chunks( enum_memory );
    clear_chunks( struct_memory );
    clear_chunks( array_memory );
    clear_chunks( range_memory );
}

dawn::NodeMemory& dawn::node_pool()
{
    return pools.node_memory;
}

dawn::FrameMemory& dawn::frame_pool()
{
    return pools.frame_memory;
}

dawn::BoolMemory& dawn::bool_pool()
{
    return pools.bool_memory;
}

dawn::IntMemory& dawn::int_pool()
{
    return pools.int_memory;
}

dawn::FloatMemory& dawn::float_pool()
{
    return pools.float_memory;
}

dawn::CharMemory& dawn::char_pool()
{
    return pools.char_memory;
}

dawn::StringMemory& dawn::string_pool()
{
    return pools.string_memory;
}

dawn::FunctionMemory& dawn::function_pool()
{
    return pools.function_memory;
}

dawn::EnumMemory& dawn::enum_pool()
{
    return pools.enum_memory;
}

dawn::StructMemory& dawn::struct_pool()
{
    return pools.struct_memory;
}

dawn::ArrayMemory& dawn::array_pool()
{
    return pools.array_memory;
}

dawn::RangeMemory& dawn::range_pool()
{
    return pools.range_memory;
}
