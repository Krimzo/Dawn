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
}();
#endif

std::wostream& dawn::operator<<( std::wostream& stream, Color const& color )
{
    stream << L"\033[38;2;" << Int( color.r ) << L";" << Int( color.g ) << L";" << Int( color.b ) << L"m";
    return stream;
}

std::wostream& dawn::operator<<( std::wostream& stream, ColoredText const& colored_text )
{
    static constexpr Color DEFAULT_COLOR = { 204, 204, 204 };
    stream << colored_text.color << colored_text.text << DEFAULT_COLOR;
    return stream;
}

dawn::Char dawn::to_escaping( Char c )
{
    switch ( c )
    {
    case L'b': return L'\b';
    case L'f': return L'\f';
    case L'n': return L'\n';
    case L'r': return L'\r';
    case L't': return L'\t';
    }
    return c;
}

dawn::String dawn::from_escaping( Char c )
{
    switch ( c )
    {
    case L'\b': return L"\\b";
    case L'\f': return L"\\f";
    case L'\n': return L"\\n";
    case L'\r': return L"\\r";
    case L'\t': return L"\\t";
    }
    return String( 1, c );
}

dawn::String dawn::read_file( StringRef const& path )
{
    IFileStream file{ path.data() };
    if ( !file )
        return {};

    return (StringStream{} << file.rdbuf()).str();
}

dawn::Opt<dawn::Int> dawn::parse_int( StringRef const& data )
{
    if ( data.empty() )
        return std::nullopt;

    Char* last_char = nullptr;
    Int result = std::wcstoll( data.data(), &last_char, 10 );
    if ( last_char != data.data() + data.size() )
        return std::nullopt;

    return { result };
}

dawn::Opt<dawn::Float> dawn::parse_float( StringRef const& data )
{
    if ( data.empty() )
        return std::nullopt;

    Char* last_char = nullptr;
    Float result = std::wcstod( data.data(), &last_char );
    if ( last_char != data.data() + data.size() )
        return std::nullopt;

    return { result };
}
