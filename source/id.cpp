#include "id.h"


dawn::Int dawn::IDSystem::get( String const& str_id )
{
    auto it = m_str_int.find( str_id );
    if ( it != m_str_int.end() )
        return it->second;

    m_str_int[str_id] = m_current;
    m_int_str[m_current] = str_id;
    return m_current++;
}

dawn::String const* dawn::IDSystem::get( Int int_id )
{
    auto it = m_int_str.find( int_id );
    if ( it != m_int_str.end() )
        return &it->second;
    return nullptr;
}

dawn::ID::ID( String str_id )
    : str_id( std::move( str_id ) )
{}

dawn::Int dawn::ID::get( IDSystem& system )
{
    if ( int_id <= 0 )
        int_id = system.get( str_id );
    return int_id;
}

std::wostream& dawn::operator<<( std::wostream& stream, ID const& id )
{
    stream << id.str_id;
    return stream;
}
