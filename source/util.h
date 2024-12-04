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

template<typename T>
struct Makeable
{
    static Ref<T> make()
    {
        return std::make_shared<T>();
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

template<typename... Args>
nullptr_t _chaos( Args const&... args )
{
    throw format( args... );
    return nullptr;
}

Char to_escaping( Char c );
String from_escaping( Char c );
String read_file( StringRef const& path );

Opt<Int> parse_int( StringRef const& data );
Opt<Float> parse_float( StringRef const& data );
}
