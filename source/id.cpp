#include "id.h"


static constexpr auto RESERVE_SIZE = 256;

dawn::Vector<dawn::String> dawn::IDSystem::m_id_str = {};
dawn::StringMap<dawn::ID> dawn::IDSystem::m_str_id = {};

dawn::ID dawn::IDSystem::get( StringRef const& str )
{
    static const auto _ = [] {
        m_id_str.reserve( RESERVE_SIZE );
        m_str_id.reserve( RESERVE_SIZE );
        m_id_str.resize( 1 ); // Because id=0 is not valid.
        return nullptr;
        }( );

    const auto it = m_str_id.find( str );
    if ( it != m_str_id.end() )
        return it->second;

    m_id_str.emplace_back( str );
    const ID id = ID( (uint32_t) m_id_str.size() - 1 );
    m_str_id.emplace( str, id );
    return id;
}

dawn::String const& dawn::IDSystem::get( ID id )
{
    return m_id_str[id.m_id];
}
