#pragma once

#include "decl.h"


namespace dawn
{
template<typename T>
struct Holder
{
    Holder() = default;

    Holder( Holder const& other )
    {
        *ptr = *other.ptr;
    }

    Holder& operator=( Holder const& other )
    {
        if ( this != &other )
            *ptr = *other.ptr;
        return *this;
    }

    T& operator*() const
    {
        return *ptr;
    }

    T* operator->() const
    {
        return ptr.get();
    }

private:
    std::unique_ptr<T> ptr = std::make_unique<T>();
};
}
