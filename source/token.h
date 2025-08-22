#pragma once

#include "util.h"


namespace dawn
{
struct Index
{
    Index() = default;

    void incr( Int n = 1 );
    void decr( Int n = 1 );
    void new_line();

    constexpr Int index() const { return m_index; }
    constexpr Int line() const { return m_line; }
    constexpr Int col() const { return m_col; }

private:
    Int m_index = 0;
    Int m_line = 1;
    Int m_col = 1;
};

struct Location
{
    Int line = 1;
    Int col = 1;

    constexpr Location()
    {
    }

    constexpr Location( Index const& index )
        : line( index.line() ), col( index.col() )
    {
    }
};

struct Token
{
    TokenType type;
    String value;
    String literal;
    Location location;
};

Color to_color( TokenType type );
Bool is_custom_type( StringRef const& value );

std::ostream& operator<<( std::ostream& stream, Location const& location );
std::ostream& operator<<( std::ostream& stream, Token const& token );
}
