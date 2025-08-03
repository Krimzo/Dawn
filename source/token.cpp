#include "token.h"


dawn::Color dawn::to_color( TokenType type )
{
    switch ( type )
    {
    case TokenType::INTEGER:  return { 240, 128, 128 };
    case TokenType::FLOAT:    return { 255, 182, 193 };
    case TokenType::CHAR:     return { 221, 160, 221 };
    case TokenType::STRING:   return { 173, 216, 230 };
    case TokenType::KEYWORD:  return { 144, 238, 144 };
    case TokenType::TYPE:     return { 255, 222, 173 };
    case TokenType::NAME:     return { 238, 221, 130 };
    case TokenType::OPERATOR: return { 211, 211, 211 };
    }
    return { 204, 204, 204 };
}

dawn::Bool dawn::is_custom_type( StringRef const& value )
{
    return !value.empty() ? isupper( value.front() ) : false;
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
        "} {" << ColoredText{ color, token.literal } <<
        "} <" << ColoredText{ color, token.line_number } << ">]";
    return stream;
}
