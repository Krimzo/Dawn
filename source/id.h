#pragma once

#include "util.h"


namespace dawn
{
struct ID
{
    friend struct IDSystem;

    constexpr ID()
    {
    }

    constexpr auto integer() const
    {
        return m_id;
    }

    constexpr auto operator==( const ID other ) const
    {
        return m_id == other.m_id;
    }

    constexpr auto operator<=>( const ID other ) const
    {
        return m_id <=> other.m_id;
    }

private:
    uint32_t m_id = 0;

    constexpr ID( const uint32_t id )
        : m_id( id )
    {
    }
};

struct IDSystem
{
    IDSystem() = delete;

    static ID get( StringRef const& str );
    static String const& get( ID id );

private:
    static Vector<String> m_id_str;
    static StringMap<ID> m_str_id;
};

constexpr uint64_t combine_ids( ID left, ID right )
{
    return ( uint64_t( left.integer() ) << 32 ) | right.integer();
}
}

namespace std
{
template<>
struct hash<dawn::ID>
{
    auto operator()( dawn::ID id ) const noexcept
    {
        return std::hash<decltype( id.integer() )>{}( id.integer() );
    }
};
}
