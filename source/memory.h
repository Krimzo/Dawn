#pragma once

#include "util.h"


namespace dawn
{
template<typename T>
struct Register
{
    Int count = 0;
    T value = {};

    void incr()
    {
        ++count;
    }

    // Does not reset value. Caller should reset after obtaining a new register.
    void decr()
    {
        --count;
    }
};

template<typename T>
struct RegisterRef
{
    RegisterRef() noexcept = default;

    RegisterRef( Register<T>* regptr ) noexcept
        : m_regptr( regptr )
    {
        if ( m_regptr )
            m_regptr->incr();
    }

    ~RegisterRef() noexcept
    {
        if ( m_regptr )
            m_regptr->decr();
    }

    RegisterRef( RegisterRef const& other ) noexcept
        : m_regptr( other.m_regptr )
    {
        if ( m_regptr )
            m_regptr->incr();
    }

    RegisterRef& operator=( RegisterRef const& other ) noexcept
    {
        if ( this != &other )
        {
            if ( m_regptr )
                m_regptr->decr();

            m_regptr = other.m_regptr;

            if ( m_regptr )
                m_regptr->incr();
        }
        return *this;
    }

    RegisterRef( RegisterRef&& other ) noexcept
        : m_regptr( std::move( other.m_regptr ) )
    {
        other.m_regptr = nullptr;
    }

    RegisterRef& operator=( RegisterRef&& other ) noexcept
    {
        if ( this != &other )
        {
            if ( m_regptr )
                m_regptr->decr();

            m_regptr = std::move( other.m_regptr );
            other.m_regptr = nullptr;
        }
        return *this;
    }

    operator Bool () const noexcept
    {
        return static_cast<Bool>( m_regptr );
    }

    T& operator*() const noexcept
    {
        return m_regptr->value;
    }

    T* operator->() const noexcept
    {
        return &m_regptr->value;
    }

    template<typename C>
    RegisterRef<C>& as()
    {
        return reinterpret_cast<RegisterRef<C>&>( *this );
    }

    template<typename C>
    RegisterRef<C> const& as() const
    {
        return reinterpret_cast<RegisterRef<C> const&>( *this );
    }

private:
    Register<T>* m_regptr = nullptr;
};

template<typename T, Int S>
struct MemoryChunk
{
    Register<T> m_space[S] = {};
    Int m_current = 0;

    // Does not reset value. Caller should reset after obtaining a new register.
    Register<T>* move_to_unused()
    {
        const Int start_index = m_current;
        while ( m_space[m_current].count != 0 )
        {
            if ( ++m_current == S )
                m_current = 0;

            if ( m_current == start_index )
                return nullptr;
        }
        return m_space + m_current;
    }
};

template<typename T, Int ChunkSize>
struct MemoryPool
{
    friend struct MemoryPools;

    MemoryPool()
    {
        m_chunks.emplace_front();
        m_current = m_chunks.begin();
    }

    // Does not reset value. Caller should reset after obtaining a new register.
    RegisterRef<T> new_register()
    {
        const auto start_iterator = m_current;
        while ( true )
        {
            if ( Register<T>* ptr = m_current->move_to_unused() )
                return ptr;

            if ( ++m_current == m_chunks.end() )
                m_current = m_chunks.begin();

            if ( m_current == start_iterator )
            {
                m_chunks.emplace_front();
                m_current = m_chunks.begin();
            }
        }
    }

private:
    List<MemoryChunk<T, ChunkSize>> m_chunks;
    List<MemoryChunk<T, ChunkSize>>::iterator m_current;
};
}
