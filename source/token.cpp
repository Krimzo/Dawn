#include "token.h"


dawn::Bool dawn::is_custom_type( StringRef const& value )
{
    if ( value.empty() )
        return false;
    return isupper( value.front() );
}

std::ostream& dawn::operator<<( std::ostream& stream, TokenType type )
{
    switch ( type )
    {
    case TokenType::INTEGER: stream << "Integer"; break;
    case TokenType::FLOAT: stream << "Float"; break;
    case TokenType::CHAR: stream << "Char"; break;
    case TokenType::STRING: stream << "String"; break;
    case TokenType::KEYWORD: stream << "Keyword"; break;
    case TokenType::TYPE: stream << "Type"; break;
    case TokenType::NAME: stream << "Name"; break;
    case TokenType::OPERATOR: stream << "Operator"; break;
    }
    return stream;
}

std::ostream& dawn::operator<<( std::ostream& stream, Token const& token )
{
    Color color = to_color( token.type );
    stream << "[(" << ColoredText{ color, token.type } <<
        ") {" << ColoredText{ color, token.value } <<
        "} {" << ColoredText{ color, token.lit_val } <<
        "} <" << ColoredText{ color, token.line_number } << ">]";
    return stream;
}
