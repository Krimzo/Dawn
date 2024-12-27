#pragma once

#include "util.h"


namespace dawn
{
struct IDSystem
{
    IDSystem() = delete;

    static Int get( String const& str_id );
    static String const& get( Int int_id );

private:
    static HMap<String, Int> m_str_int;
    static OMap<Int, String> m_int_str;
    static Int m_current;
};
}
