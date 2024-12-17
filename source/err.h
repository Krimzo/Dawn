#pragma once

#include "token.h"


namespace dawn
{
struct LexerError
{
    String msg;

    template<typename... Args>
    LexerError( Int line_number, Char c, Args&&... args )
    {
        StringStream stream;
        stream << "Lexer error at line [" << line_number
            << "] and char [" << from_escaping( c ) << "]: ";
        (stream << ... << args);
        msg = stream.str();
    }
};

struct ParserError
{
    String msg;

    template<typename... Args>
    ParserError( Opt<Token> const& token, Args&&... args )
    {
        StringStream stream;
        if ( token )
        {
            stream << "Parser error at token " << *token << ": ";
        }
        else
        {
            stream << "Parser error: ";
        }
        (stream << ... << args);
        msg = stream.str();
    }
};

struct EngineError
{
    String msg;

    template<typename... Args>
    EngineError( Args&&... args )
    {
        StringStream stream;
        stream << "Engine error: ";
        (stream << ... << args);
        msg = stream.str();
    }
};

std::ostream& operator<<( std::ostream& stream, LexerError const& error );
std::ostream& operator<<( std::ostream& stream, ParserError const& error );
std::ostream& operator<<( std::ostream& stream, EngineError const& error );

#define PANIC(...) throw format( "Error: ", __VA_ARGS__ )
#define LEXER_PANIC(...) throw format( LexerError{ __VA_ARGS__ } )
#define PARSER_PANIC(...) throw format( ParserError{ __VA_ARGS__ } )
#define ENGINE_PANIC(...) throw format( EngineError{ __VA_ARGS__ } )
}
