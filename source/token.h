#pragma once

#include "util.h"


namespace dawn
{
enum struct TokenType
{
    INTEGER = 0,
    FLOAT,
    CHAR,
    STRING,
    KEYWORD,
    TYPE,
    FUNCTION,
    NAME,
    OPERATOR,
};

struct Token
{
    TokenType type;
    String value;
    String lit_val;
    Int line_number = 0;

    Bool is_custom_type() const;
};

constexpr Color to_color( TokenType type )
{
    switch ( type )
    {
    case TokenType::INTEGER:  return { 240, 128, 128 };
    case TokenType::FLOAT:    return { 255, 182, 193 };
    case TokenType::CHAR:     return { 221, 160, 221 };
    case TokenType::STRING:   return { 173, 216, 230 };
    case TokenType::KEYWORD:  return { 144, 238, 144 };
    case TokenType::TYPE:     return { 255, 222, 173 };
    case TokenType::FUNCTION: return { 175, 238, 238 };
    case TokenType::NAME:     return { 238, 221, 130 };
    case TokenType::OPERATOR: return { 211, 211, 211 };
    }
    return { 204, 204, 204 };
}

std::ostream& operator<<( std::ostream& stream, TokenType type );
std::ostream& operator<<( std::ostream& stream, Token const& token );
}
