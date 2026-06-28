#pragma once

#include "util.h"


namespace dawn
{
struct ID
{
    using IntType = uint32_t;

    constexpr ID()
    {}

    ID( char const* str )
    {
        this->init( str );
    }

    ID( String const& str )
    {
        this->init( str );
    }

    ID( StringRef const& str )
    {
        this->init( str );
    }

    void init( StringRef const& str )
    {
        const auto it = STR_ID.find( str );
        if ( it != STR_ID.end() )
            m_id = it->second;
        else
        {
            ID_STR.emplace_back( str );
            m_id = (IntType) ID_STR.size() - 1;
            STR_ID.emplace( str, m_id );
        }
    }

    constexpr IntType integer() const
    {
        return m_id;
    }

    String const& string() const
    {
        return ID_STR[m_id];
    }

    constexpr Bool valid() const
    {
        return m_id != 0;
    }

    constexpr Bool operator==( const ID other ) const
    {
        return m_id == other.m_id;
    }

    constexpr auto operator<=>( const ID other ) const
    {
        return m_id <=> other.m_id;
    }

    friend std::ostream& operator<<( std::ostream& stream, ID id )
    {
        stream << id.string();
        return stream;
    }

private:
    static inline Vector<String> ID_STR = {};
    static inline StringMap<IntType> STR_ID = {};
    static constexpr auto RESERVE_SIZE = 256;
    static inline const auto _ = [] {
        ID_STR.reserve( RESERVE_SIZE );
        STR_ID.reserve( RESERVE_SIZE );
        ID_STR.resize( 1 ); // Because id=0 is not valid.
        return nullptr;
        }( );
    IntType m_id = 0;
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
