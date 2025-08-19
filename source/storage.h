#pragma once

#include "t.h"


namespace dawn
{
template<typename T>
struct LocalStorage
{
    LocalStorage( Int reserver_size = 0 )
    {
        m_data.reserve( reserver_size );
    }

    T& set( Int id, T const& t )
    {
        return m_data.emplace_back( id, t ).second;
    }

    T* get( Int id )
    {
        for ( auto& [obj_id, obj] : m_data )
        {
            if ( obj_id == id )
                return &obj;
        }
        return nullptr;
    }

    void clear()
    {
        m_data.clear();
    }

private:
    Vector<Pair<Int, T>> m_data;
};

template<typename T>
struct GlobalStorage
{
    GlobalStorage( Int initial_size = 0 )
    {
        m_data.resize( initial_size );
    }

    T& set( Int id, T const& t )
    {
        if ( (Int) m_data.size() <= id )
            m_data.resize( ( id + 1 ) * 2 );
        return m_data[id].emplace( t );
    }

    T* get( Int id )
    {
        if ( (Int) m_data.size() > id )
        {
            if ( auto& opt_obj = m_data[id] )
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
