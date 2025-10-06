#pragma once

#include "memory.h"
#include "node.h"
#include "stack.h"


namespace dawn
{
using NodeMemory = MemoryPool<Node, 1024>;
using FrameMemory = MemoryPool<Frame, 256>;
using BoolMemory = MemoryPool<ValueStorage<Bool>, 1024>;
using IntMemory = MemoryPool<ValueStorage<Int>, 1024>;
using FloatMemory = MemoryPool<ValueStorage<Float>, 1024>;
using CharMemory = MemoryPool<ValueStorage<Char>, 1024>;
using StringMemory = MemoryPool<ValueStorage<String>, 1024>;
using FunctionMemory = MemoryPool<ValueStorage<FunctionValue>, 1024>;
using EnumMemory = MemoryPool<ValueStorage<EnumValue>, 1024>;
using StructMemory = MemoryPool<ValueStorage<StructValue>, 1024>;
using ArrayMemory = MemoryPool<ValueStorage<ArrayValue>, 1024>;
using RangeMemory = MemoryPool<ValueStorage<RangeValue>, 1024>;

struct MemoryPools
{
    NodeMemory node_memory;
    FrameMemory frame_memory;
    BoolMemory bool_memory;
    IntMemory int_memory;
    FloatMemory float_memory;
    CharMemory char_memory;
    StringMemory string_memory;
    FunctionMemory function_memory;
    EnumMemory enum_memory;
    StructMemory struct_memory;
    ArrayMemory array_memory;
    RangeMemory range_memory;

    ~MemoryPools() noexcept;
};

NodeMemory& node_pool();
FrameMemory& frame_pool();
BoolMemory& bool_pool();
IntMemory& int_pool();
FloatMemory& float_pool();
CharMemory& char_pool();
StringMemory& string_pool();
FunctionMemory& function_pool();
EnumMemory& enum_pool();
StructMemory& struct_pool();
ArrayMemory& array_pool();
RangeMemory& range_pool();
}
