#pragma once

#include "util.h"


namespace dawn
{
struct Bad
{
};

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
    using IntType = int32_t;

    String file_path;
    IntType line = 1;
    IntType col = 1;

    explicit constexpr Location()
    {
    }

    explicit constexpr Location( String path, Index const& index )
        : file_path( std::move( path ) ), line( (IntType) index.line() ), col( (IntType) index.col() )
    {
    }

    explicit constexpr Location( Bad bad )
        : line( -1 ), col( -1 )
    {
    }
};

struct Token
{
    TokenType type;
    String value;
    String literal;
    Location location{};

    constexpr String const& any_value() const
    {
        return value.empty() ? literal : value;
    }
};

Color to_color( TokenType type );
Bool is_custom_type( StringRef const& value );

std::ostream& operator<<( std::ostream& stream, Location const& location );
std::ostream& operator<<( std::ostream& stream, Token const& token );
}
