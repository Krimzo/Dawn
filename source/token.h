#pragma once

#include "util.h"


namespace dawn
{
struct Token
{
    TokenType type;
    String value;
    Int line_number = 0;
};

Color to_color( TokenType type );
Bool is_custom_type( StringRef const& value );

std::ostream& operator<<( std::ostream& stream, Token const& token );
}
