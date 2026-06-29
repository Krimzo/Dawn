#pragma once

#include "t.h"


namespace dawn
{
template<typename T>
struct Storage
{
    Storage( size_t initial_size = 0 )
    {
        m_data.resize( initial_size );
    }

    T& set( ID id, T const& t )
    {
        if ( m_data.size() <= id.integer() )
            m_data.resize( ( id.integer() + 1 ) * 2 );
        return m_data[id.integer()].emplace( t );
    }

    T* get( ID id )
    {
        if ( m_data.size() > id.integer() )
        {
            if ( auto& opt_obj = m_data[id.integer()] )
                return &( *opt_obj );
        }
        return nullptr;
    }

    void clear()
    {
        m_data.clear();
    }

private:
    Vector<Opt<T>> m_data;
};
}
