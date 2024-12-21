#include "memory.h"
#include "values.h"
#include "stack.h"


dawn::GlobalMemory::~GlobalMemory() noexcept
{
    for ( auto& [scope, _] : this->scope_memory.m_space )
    {
        scope.objects.clear();
        scope.parent = {};
    }
}

dawn::GlobalMemory& dawn::get_global_memory()
{
    static GlobalMemory instance;
    return instance;
}
