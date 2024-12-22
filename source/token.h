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
    OPERATOR,
};

struct Token
{
    TokenType type;
    String value;
    String lit_val;
    Int line_number = 0;
};

Color to_color( TokenType type );
Bool is_custom_type( StringRef const& value );

std::ostream& operator<<( std::ostream& stream, TokenType type );
std::ostream& operator<<( std::ostream& stream, Token const& token );
}
