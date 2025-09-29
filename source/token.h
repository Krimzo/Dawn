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
    using IntType = int32_t;

    String file_path;
    IntType line = 1;
    IntType col = 1;

    explicit constexpr Location()
    {
    }

    explicit constexpr Location( IntType line, IntType col )
        : line( line ), col( col )
    {
    }

    explicit constexpr Location( String path, Index const& index )
        : file_path( std::move( path ) ), line( (IntType) index.line() ), col( (IntType) index.col() )
    {
    }

    inline String to_string( Opt<Color> const& color ) const
    {
        StringStream stream;
        if ( color )
            stream << "(" << ColoredText{ *color, file_path } << ")<" << ColoredText{ *color, line } << ", " << ColoredText{ *color, col } << ">";
        else
            stream << "(" << file_path << ")<" << line << ", " << col << ">";
        return stream.str();
    }

    static const Location none;
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

std::ostream& operator<<( std::ostream& stream, Token const& token );
}
