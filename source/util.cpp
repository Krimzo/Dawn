#include "util.h"


#ifdef _WIN32
#include <windows.h>
static HANDLE _console_init = []
    {
        DWORD mode = 0;
        HANDLE handle = GetStdHandle( STD_OUTPUT_HANDLE );
        GetConsoleMode( handle, &mode );
        SetConsoleMode( handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING );
        return handle;
    }( );
#endif

dawn::Char dawn::to_escaping( Char c )
{
    switch ( c )
    {
    case 'b': return '\b';
    case 'f': return '\f';
    case 'n': return '\n';
    case 'r': return '\r';
    case 't': return '\t';
    }
    return c;
}

dawn::String dawn::from_escaping( Char c )
{
    switch ( c )
    {
    case '\b': return "\\b";
    case '\f': return "\\f";
    case '\n': return "\\n";
    case '\r': return "\\r";
    case '\t': return "\\t";
    }
    return String( 1, c );
}

dawn::Opt<dawn::String> dawn::read_file( StringRef const& path )
{
    IFileStream file{ path.data() };
    if ( !file )
        return std::nullopt;
    return { ( StringStream{} << file.rdbuf() ).str() };
}

dawn::Opt<dawn::Int> dawn::parse_int( StringRef const& data )
{
    if ( data.empty() )
        return std::nullopt;

    Char* last_char = nullptr;
    Int result = std::strtoll( data.data(), &last_char, 10 );
    if ( last_char != data.data() + data.size() )
        return std::nullopt;

    return { result };
}

dawn::Opt<dawn::Float> dawn::parse_float( StringRef const& data )
{
    if ( data.empty() )
        return std::nullopt;

    Char* last_char = nullptr;
    Float result = std::strtod( data.data(), &last_char );
    if ( last_char != data.data() + data.size() )
        return std::nullopt;

    return { result };
}

dawn::Float dawn::dawn_mod( Float left, Float right )
{
    if ( left < 0.0 ) return ( right < 0.0 ) ? std::fmod( left, right ) : std::remainder( left, right );
    else              return ( right < 0.0 ) ? std::remainder( left, right ) : std::fmod( left, right );
}

std::ostream& dawn::operator<<( std::ostream& stream, Color const& color )
{
    stream << "\033[38;2;" << Int( color.r ) << ";" << Int( color.g ) << ";" << Int( color.b ) << "m";
    return stream;
}

std::ostream& dawn::operator<<( std::ostream& stream, ColoredText const& colored_text )
{
    static constexpr Color DEFAULT_COLOR = { 204, 204, 204 };
    stream << colored_text.color << colored_text.text << DEFAULT_COLOR;
    return stream;
}
