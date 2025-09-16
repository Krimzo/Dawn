#include "pool.h"


static dawn::MemoryPools pools;

dawn::MemoryPools::~MemoryPools() noexcept
{
    constexpr auto clear_chunks = []( auto& pool )
        {
            for ( auto& chunk : pool.m_chunks )
            {
                for ( auto& regist : chunk.m_space )
                    regist.value = {};
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

dawn::MemoryPool<dawn::Node, 1024>& dawn::node_pool()
{
    return pools.node_memory;
}

dawn::MemoryPool<dawn::Frame, 256>& dawn::frame_pool()
{
    return pools.frame_memory;
}

dawn::MemoryPool<dawn::Bool, 1024>& dawn::bool_pool()
{
    return pools.bool_memory;
}

dawn::MemoryPool<dawn::Int, 1024>& dawn::int_pool()
{
    return pools.int_memory;
}

dawn::MemoryPool<dawn::Float, 1024>& dawn::float_pool()
{
    return pools.float_memory;
}

dawn::MemoryPool<dawn::Char, 1024>& dawn::char_pool()
{
    return pools.char_memory;
}

dawn::MemoryPool<dawn::String, 1024>& dawn::string_pool()
{
    return pools.string_memory;
}

dawn::MemoryPool<dawn::FunctionValue, 1024>& dawn::function_pool()
{
    return pools.function_memory;
}

dawn::MemoryPool<dawn::EnumValue, 1024>& dawn::enum_pool()
{
    return pools.enum_memory;
}

dawn::MemoryPool<dawn::StructValue, 1024>& dawn::struct_pool()
{
    return pools.struct_memory;
}

dawn::MemoryPool<dawn::ArrayValue, 1024>& dawn::array_pool()
{
    return pools.array_memory;
}

dawn::MemoryPool<dawn::RangeValue, 1024>& dawn::range_pool()
{
    return pools.range_memory;
}
