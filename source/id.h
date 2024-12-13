#pragma once

#include "util.h"


namespace dawn
{
struct IDSystem
{
    Int get( String const& str_id );
    String const* get( Int int_id );

private:
    Map<String, Int> m_str_int;
    Map<Int, String> m_int_str;
    Int m_current = 1;
};

struct ID
{
    String str_id;

    ID() = default;
    ID( String str_id );

    Int get( IDSystem& system );

private:
    Int int_id = 0;
};

std::wostream& operator<<( std::wostream& stream, ID const& id );
}
