#pragma once

#include "util.h"


namespace dawn
{
template<size_t S, size_t A, typename E, typename H>
struct Storage
{
    Storage() = default;

    ~Storage() noexcept
    {
        reset();
    }

    Storage( Storage const& other )
        : m_type( other.m_type )
    {
        H::copy( m_type, m_buffer, other.m_buffer );
    }

    Storage& operator=( Storage const& other )
    {
        if ( this != &other )
        {
            reset();
            m_type = other.m_type;
            H::copy( m_type, m_buffer, other.m_buffer );
        }
        return *this;
    }

    template<typename T, typename... Args>
        requires (alignof(T) <= A and sizeof( T ) <= S)
    T& emplace( Args&&... args )
    {
        reset();
        m_type = H::template type<T>();
        return *new (m_buffer) T( args... );
    }

    E type() const noexcept
    {
        return m_type;
    }

    template<typename T>
    T& as() noexcept
    {
        return *reinterpret_cast<T*>(m_buffer);
    }

    template<typename T>
    T const& as() const noexcept
    {
        return *reinterpret_cast<T const*>(m_buffer);
    }

    void reset()
    {
        H::destruct( m_type, m_buffer );
        m_type = E( 0 );
    }

private:
    alignas(A) std::byte m_buffer[S] = {};
    E m_type = E( 0 );
};

template<typename E, typename H>
struct DynStorage
{
    DynStorage() = default;

    ~DynStorage() noexcept
    {
        reset();
    }

    DynStorage( DynStorage const& other )
        : m_type( other.m_type )
    {
        H::copy( m_type, m_ptr, other.m_ptr );
    }

    DynStorage& operator=( DynStorage const& other )
    {
        if ( this != &other )
        {
            reset();
            m_type = other.m_type;
            H::copy( m_type, m_ptr, other.m_ptr );
        }
        return *this;
    }

    template<typename T, typename... Args>
    T& emplace( Args&&... args )
    {
        reset();
        m_type = H::template type<T>();
        m_ptr = new T( args... );
        return *static_cast<T*>(m_ptr);
    }

    E type() const noexcept
    {
        return m_type;
    }

    template<typename T>
    T& as() noexcept
    {
        return *static_cast<T*>(m_ptr);
    }

    template<typename T>
    T const& as() const noexcept
    {
        return *static_cast<T const*>(m_ptr);
    }

    void reset()
    {
        H::destruct( m_type, m_ptr );
        m_type = E( 0 );
    }

private:
    void* m_ptr = nullptr;
    E m_type = E( 0 );
};
}
