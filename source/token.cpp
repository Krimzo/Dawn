#include "token.h"


dawn::Bool dawn::Token::is_custom_type() const
{
    if ( value.empty() )
        return false;
    return iswupper( value.front() );
}

std::wostream& dawn::operator<<( std::wostream& stream, TokenType type )
{
    switch ( type )
    {
    case TokenType::INTEGER: stream << L"Integer"; break;
    case TokenType::FLOAT: stream << L"Float"; break;
    case TokenType::CHAR: stream << L"Char"; break;
    case TokenType::STRING: stream << L"String"; break;
    case TokenType::KEYWORD: stream << L"Keyword"; break;
    case TokenType::TYPE: stream << L"Type"; break;
    case TokenType::FUNCTION: stream << L"Function"; break;
    case TokenType::NAME: stream << L"Name"; break;
    case TokenType::OPERATOR: stream << L"Operator"; break;
    }
    return stream;
}

std::wostream& dawn::operator<<( std::wostream& stream, Token const& token )
{
    Color color = to_color( token.type );
    stream << L"[(" << ColoredText{ color, token.type } <<
        L") {" << ColoredText{ color, token.value } <<
        L"} {" << ColoredText{ color, token.lit_val } <<
        L"} <" << ColoredText{ color, token.line_number } << L">]";
    return stream;
}
