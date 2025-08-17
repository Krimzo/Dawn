#include "id.h"


dawn::Vector<dawn::String> dawn::IDSystem::m_int_str = {};
dawn::HMap<dawn::String, dawn::Int> dawn::IDSystem::m_str_int = {};

dawn::Int dawn::IDSystem::get( String const& str_id )
{
    static const auto _ = [] {
        m_int_str.reserve( 250 );
        m_int_str.resize( 1 ); // because id=0 is not valid
        return nullptr;
        }( );

    auto it = m_str_int.find( str_id );
    if ( it != m_str_int.end() )
        return it->second;

    m_int_str.emplace_back( str_id );
    const Int int_id = static_cast<Int>( m_int_str.size() - 1 );
    m_str_int.emplace( str_id, int_id );
    return int_id;
}

dawn::String const& dawn::IDSystem::get( Int int_id )
{
    return m_int_str[int_id];
}
