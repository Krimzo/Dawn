#pragma once

#include "util.h"


namespace dawn
{
template<typename T>
struct Stack
{
    Stack( Int reserve = 512 )
    {
        m_data.reserve( reserve );
    }

    void push( Int id, T const& val )
    {
        m_data.emplace_back( id, val );
    }

    void pop( Int n = 1 )
    {
        m_data.resize( m_data.size() - n );
    }

    T* get( Int id )
    {
        for ( auto it = m_data.rbegin(); it != m_data.rend(); ++it )
        {
            if ( it->first == id )
                return &it->second;
        }
        return nullptr;
    }

private:
    Array<Pair<Int, T>> m_data;
};
}
