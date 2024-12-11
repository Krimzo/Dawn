#pragma once

#include "token.h"


namespace dawn
{
struct LexerError
{
    String msg;

    template<typename... Args>
    LexerError( Int line_number, Char c, Args const&... args )
    {
        StringStream stream;
        stream << L"Lexer error at line [" << line_number
            << L"] and char [" << from_escaping( c ) << L"]: ";
        (stream << ... << args);
        msg = stream.str();
    }
};

struct ParserError
{
    String msg;

    template<typename... Args>
    ParserError( Opt<Token> const& token, Args const&... args )
    {
        StringStream stream;
        if ( token )
        {
            stream << L"Parser error at token " << *token << L": ";
        }
        else
        {
            stream << L"Parser error: ";
        }
        (stream << ... << args);
        msg = stream.str();
    }
};

struct EngineError
{
    String msg;

    template<typename... Args>
    EngineError( Args const&... args )
    {
        StringStream stream;
        stream << "Engine error: ";
        (stream << ... << args);
        msg = stream.str();
    }
};

std::wostream& operator<<( std::wostream& stream, LexerError const& error );
std::wostream& operator<<( std::wostream& stream, ParserError const& error );
std::wostream& operator<<( std::wostream& stream, EngineError const& error );

#define PANIC(...) throw format( "Error: ", __VA_ARGS__ );
#define LEXER_PANIC(...) throw format( LexerError{ __VA_ARGS__ } );
#define PARSER_PANIC(...) throw format( ParserError{ __VA_ARGS__ } );
#define ENGINE_PANIC(...) throw format( EngineError{ __VA_ARGS__ } );
}
