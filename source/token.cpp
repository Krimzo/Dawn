#include "token.h"


const dawn::Location dawn::Location::none{ -1, -1 };

void dawn::Index::incr( Int n )
{
    m_index += n;
    m_col += n;
}

void dawn::Index::decr( Int n )
{
    m_index -= n;
    m_col -= n;
}

void dawn::Index::new_line()
{
    m_line += 1;
    m_col = 1;
}

dawn::Color dawn::to_color( TokenType type )
{
    switch ( type )
    {
    case TokenType::INTEGER:  return { 240, 128, 128 };
    case TokenType::FLOAT:    return { 255, 182, 193 };
    case TokenType::CHAR:     return { 221, 160, 221 };
    case TokenType::STRING:   return { 127, 197, 219 };
    case TokenType::KEYWORD:  return { 144, 238, 144 };
    case TokenType::TYPE:     return { 255, 222, 173 };
    case TokenType::NAME:     return { 238, 221, 130 };
    case TokenType::OPERATOR: return { 191, 110, 204 };
    }
    return { 204, 204, 204 };
}

dawn::Bool dawn::is_custom_type( StringRef const& value )
{
    return !value.empty() ? std::isupper( value.front() ) : false;
}

std::ostream& dawn::operator<<( std::ostream& stream, Token const& token )
{
    const Color color = to_color( token.type );
    stream << ColoredText{ color, token.type } << token.location.to_string( color ) <<
        "[" << ColoredText{ color, !token.literal.empty() ? token.literal : token.value } << "]";
    return stream;
}
