#pragma once

#include "decl.h"


namespace dawn
{
#define SALLOC(T, N) static_cast<T*>(_alloca((N) * sizeof(T)))

template<typename T>
struct SAllocManager
{
    T* ptr;
    Int count;

    SAllocManager( T* ptr, Int count )
        : ptr( ptr ), count( count )
    {
        for ( Int i = 0; i < count; i++ )
            new ( ptr + i ) T();
    }

    ~SAllocManager() noexcept
    {
        for ( Int i = 0; i < count; i++ )
            ptr[i].~T();
    }

    SAllocManager( SAllocManager const& ) = delete;
    void operator=( SAllocManager const& ) = delete;
};
}
