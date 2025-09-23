#pragma once

#include "token.h"


namespace dawn
{
struct LexerError
{
    String msg;

    template<typename... Args>
    explicit LexerError( Location const& location, Char c, Args&&... args )
    {
        StringStream stream;
        stream << "Lexer error at location " << location
            << " and char [" << from_escaping( c ) << "]: ";
        ( stream << ... << args );
        msg = stream.str();
    }
};

struct ParserError
{
    String msg;

    template<typename... Args>
    explicit ParserError( Token const& token, Args&&... args )
    {
        StringStream stream;
        stream << "Parser error at token " << token << ": ";
        ( stream << ... << args );
        msg = stream.str();
    }
};

struct EngineError
{
    String msg;

    template<typename... Args>
    explicit EngineError( Location const& location, Args&&... args )
    {
        StringStream stream;
        stream << "Engine error at location " << location << ": ";
        ( stream << ... << args );
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
