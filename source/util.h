#pragma once

#include "t.h"


namespace dawn
{
struct Color
{
    uint8_t r, g, b;
};

std::wostream& operator<<( std::wostream& stream, Color const& color );

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

std::wostream& operator<<( std::wostream& stream, ColoredText const& colored_text );

Char to_escaping( Char c );
String from_escaping( Char c );
String read_file( StringRef const& path );
}
