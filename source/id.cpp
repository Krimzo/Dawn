#include "id.h"


dawn::Map<dawn::String, dawn::Int> dawn::IDSystem::m_str_int = {};
dawn::Map<dawn::Int, dawn::String> dawn::IDSystem::m_int_str = {};
dawn::Int dawn::IDSystem::m_current = 0;

dawn::Int dawn::IDSystem::get( String const& str_id )
{
    auto it = m_str_int.find( str_id );
    if ( it != m_str_int.end() )
        return it->second;

    ++m_current;
    m_str_int[str_id] = m_current;
    m_int_str[m_current] = str_id;
    return m_current;
}

dawn::String const& dawn::IDSystem::get( Int int_id )
{
    return m_int_str[int_id];
}
