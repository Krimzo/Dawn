#pragma once

#include "util.h"


namespace dawn
{
struct IDSystem
{
    IDSystem() = delete;

    static Int get( StringRef const& str_id );
    static String const& get( Int int_id );

private:
    static Vector<String> m_int_str;
    static StringMap<Int> m_str_int;
};
}
