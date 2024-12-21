#include "memory.h"
#include "values.h"
#include "stack.h"


dawn::GlobalMemory::~GlobalMemory() noexcept
{
    for ( auto& value : value_memory.m_space )
    {
        if ( value.count <= 0 )
            continue;

        if ( value.value.type() != ValueType::FUNCTION )
            continue;

        auto& func = value.value.as<Function>();
        func.lambda_parent = {};
    }
}

dawn::GlobalMemory& dawn::get_global_memory()
{
    static GlobalMemory instance;
    return instance;
}
