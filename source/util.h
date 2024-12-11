#pragma once

#include "t.h"


namespace dawn
{
struct Color
{
    uint8_t r, g, b;
};

struct ColoredText
{
    Color color;
    String text;

    template<typename... Args>
    ColoredText( Color color, Args const&... args )
        : color( color )
    {
        StringStream stream;
        (stream << ... << args);
        text = stream.str();
    }
};

template<typename... Args>
String format( Args const&... args )
{
    StringStream stream;
    (stream << ... << args);
    return stream.str();
}

template<typename... Args>
void print( Args const&... args )
{
    (std::wcout << ... << args) << '\n';
}

Char to_escaping( Char c );
String from_escaping( Char c );
Opt<String> read_file( StringRef const& path );

Opt<Int> parse_int( StringRef const& data );
Opt<Float> parse_float( StringRef const& data );

Float mymod( Float left, Float right );

std::wostream& operator<<( std::wostream& stream, Color const& color );
std::wostream& operator<<( std::wostream& stream, ColoredText const& colored_text );
}
