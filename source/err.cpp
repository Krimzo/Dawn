#include "err.h"


std::ostream& dawn::operator<<( std::ostream& stream, LexerError const& error )
{
    stream << error.msg;
    return stream;
}

std::ostream& dawn::operator<<( std::ostream& stream, ParserError const& error )
{
    stream << error.msg;
    return stream;
}

std::ostream& dawn::operator<<( std::ostream& stream, EngineError const& error )
{
    stream << error.msg;
    return stream;
}
