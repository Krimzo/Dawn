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
    NAME,
    CALL,
    INDEX,
    OPERATOR,
};

struct Token
{
    TokenType type;
    String value;
    String lit_val;
    Int line_number = 0;
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
    case TokenType::NAME:     return { 238, 221, 130 };
    case TokenType::CALL:     return { 175, 238, 238 };
    case TokenType::INDEX:    return { 175, 238, 238 };
    case TokenType::OPERATOR: return { 211, 211, 211 };
    }
    return { 204, 204, 204 };
}

Bool is_custom_type( StringRef const& value );

std::ostream& operator<<( std::ostream& stream, TokenType type );
std::ostream& operator<<( std::ostream& stream, Token const& token );
}
