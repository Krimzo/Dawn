#pragma once

#include "t.h"


namespace dawn
{
template<typename T>
struct Storage
{
    constexpr Storage()
    {}

    T& set( ID id, T const& t )
    {
        if ( id.integer() >= m_data.size() )
            m_data.resize( size_t( id.integer() + 1 ) * 2 );
        return m_data[id.integer()].emplace( t );
    }

    T* get( ID id )
    {
        if ( id.integer() < m_data.size() )
        {
            if ( auto& opt_obj = m_data[id.integer()] )
                return &( *opt_obj );
        }
        return nullptr;
    }

private:
    Vector<Opt<T>> m_data;
};
}
