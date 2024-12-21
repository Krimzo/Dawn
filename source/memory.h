#pragma once

#include "util.h"


namespace dawn
{
template<typename T>
struct Memory;

template<typename T>
struct Register
{
    T value = {};
    Int count = 0;

    void incr()
    {
        ++count;
    }

    void decr()
    {
        --count;
    }
};

template<typename T>
struct RegisterRef
{
    RegisterRef() noexcept = default;

    RegisterRef( Memory<T>* memory, Int index ) noexcept
        : m_memory( memory ), m_index( index )
    {
        if ( valid() )
            regist().incr();
    }

    ~RegisterRef() noexcept
    {
        if ( valid() )
            regist().decr();
    }

    RegisterRef( RegisterRef const& other ) noexcept
        : m_memory( other.m_memory ), m_index( other.m_index )
    {
        if ( valid() )
            regist().incr();
    }

    RegisterRef& operator=( RegisterRef const& other ) noexcept
    {
        if ( this != &other )
        {
            if ( valid() )
                regist().decr();

            m_memory = other.m_memory;
            m_index = other.m_index;

            if ( valid() )
                regist().incr();
        }
        return *this;
    }

    constexpr Bool valid() const noexcept
    {
        return m_memory;
    }

    T& value() const noexcept
    {
        return regist().value;
    }

private:
    Memory<T>* m_memory = nullptr;
    Int m_index = -1;

    Register<T>& regist() const noexcept
    {
        return m_memory->m_space[m_index];
    }
};

template<typename T>
struct Memory
{
    friend struct GlobalMemory;
    friend struct RegisterRef<T>;

    Memory( Int initial_count = 1 )
    {
        m_space.resize( std::max( initial_count, Int( 1 ) ) );
    }

    RegisterRef<T> new_register()
    {
        move_to_unused();
        return { this, m_current };
    }

private:
    Array<Register<T>> m_space;
    Int m_current = 0;

    void move_to_unused()
    {
        Int first_index = m_current;
        while ( m_space[m_current].count > 0 )
        {
            increment_current();
            if ( m_current == first_index )
                allocate_more();
        }
    }

    void increment_current()
    {
        if ( ++m_current >= (Int) m_space.size() )
            m_current = 0;
    }

    void allocate_more()
    {
        m_space.resize( m_space.size() * 2 );
    }
};

struct Value;
struct ScopeObject;

struct GlobalMemory
{
    Memory<ScopeObject> scope_memory{ 128 };
    Memory<Value> value_memory{ 2048 };

    ~GlobalMemory() noexcept;
};

GlobalMemory& get_global_memory();
}
