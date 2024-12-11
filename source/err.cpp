#include "err.h"


std::wostream& dawn::operator<<( std::wostream& stream, LexerError const& error )
{
    stream << error.msg;
    return stream;
}

std::wostream& dawn::operator<<( std::wostream& stream, ParserError const& error )
{
    stream << error.msg;
    return stream;
}

std::wostream& dawn::operator<<( std::wostream& stream, EngineError const& error )
{
    stream << error.msg;
    return stream;
}
