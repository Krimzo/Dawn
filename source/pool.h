#pragma once

#include "memory.h"
#include "node.h"
#include "stack.h"


namespace dawn
{
struct MemoryPools
{
    MemoryPool<Frame, 256> frame_memory;
    MemoryPool<Node, 1024> node_memory;

    MemoryPool<Bool, 1024> bool_memory;
    MemoryPool<Int, 1024> int_memory;
    MemoryPool<Float, 1024> float_memory;
    MemoryPool<Char, 1024> char_memory;
    MemoryPool<String, 1024> string_memory;

    MemoryPool<Function, 1024> function_memory;
    MemoryPool<EnumValue, 1024> enum_memory;
    MemoryPool<StructValue, 1024> struct_memory;
    MemoryPool<ArrayValue, 1024> array_memory;
    MemoryPool<RangeValue, 1024> range_memory;

    ~MemoryPools() noexcept;
};

MemoryPool<Frame, 256>& frame_pool();
MemoryPool<Node, 1024>& node_pool();

MemoryPool<Bool, 1024>& bool_pool();
MemoryPool<Int, 1024>& int_pool();
MemoryPool<Float, 1024>& float_pool();
MemoryPool<Char, 1024>& char_pool();
MemoryPool<String, 1024>& string_pool();

MemoryPool<Function, 1024>& function_pool();
MemoryPool<EnumValue, 1024>& enum_pool();
MemoryPool<StructValue, 1024>& struct_pool();
MemoryPool<ArrayValue, 1024>& array_pool();
MemoryPool<RangeValue, 1024>& range_pool();
}
