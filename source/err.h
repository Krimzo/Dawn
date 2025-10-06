#pragma once

#include "token.h"


namespace dawn
{
struct LexerError
{
    static constexpr Color COLOR = { 250, 102, 102 };

    String msg;

    template<typename... Args>
    explicit LexerError( Location const& location, Char c, Args&&... args )
    {
        StringStream stream;
        stream << ColoredText{ COLOR, "Lexer" } << " error at char " << location.to_string( COLOR ) << "[" << ColoredText{ COLOR, from_escaping( c ) } << "]: ";
        stream << ColoredText{ COLOR, args... };
        msg = stream.str();
    }
};

struct ParserError
{
    static constexpr Color COLOR = { 255, 207, 116 };

    String msg;

    template<typename... Args>
    explicit ParserError( Token const& token, Args&&... args )
    {
        StringStream stream;
        stream << ColoredText{ COLOR, "Parser" } << " error at token " << token << ": ";
        stream << ColoredText{ COLOR, args... };
        msg = stream.str();
    }
};

struct EngineError
{
    static constexpr Color COLOR = { 255, 149, 85 };

    String msg;

    template<typename... Args>
    explicit EngineError( Location const& location, Args&&... args )
    {
        StringStream stream;
        stream << ColoredText{ COLOR, "Engine" } << " error at " << location.to_string( COLOR ) << ": ";
        stream << ColoredText{ COLOR, args... };
        msg = stream.str();
    }
};

std::ostream& operator<<( std::ostream& stream, LexerError const& error );
std::ostream& operator<<( std::ostream& stream, ParserError const& error );
std::ostream& operator<<( std::ostream& stream, EngineError const& error );

#define LEXER_PANIC(...) throw format( LexerError{ __VA_ARGS__ } )
#define PARSER_PANIC(...) throw format( ParserError{ __VA_ARGS__ } )
#define ENGINE_PANIC(...) throw format( EngineError{ __VA_ARGS__ } )
}
