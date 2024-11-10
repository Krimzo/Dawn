#pragma once

#include "syntax.h"


namespace dawn
{
struct LexError
{
    String msg;

    template<typename... Args>
    LexError( Int line_number, Char c, Args const&... args )
    {
        StringStream stream;
        stream << "Lex error at line [" << line_number
            << "] and char [" << from_escaping( c ) << "]: ";
        (stream << ... << args);
        msg = stream.str();
    }

    inline operator String const& () const
    {
        return msg;
    }
};

std::wostream& operator<<( std::wostream& stream, LexError const& error );
}

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

std::wostream& operator<<( std::wostream& stream, TokenType type );
}

namespace dawn
{
struct Token
{
    TokenType type;
    String value;
    Int line_number = 0;
};

std::wostream& operator<<( std::wostream& stream, Token const& token );
}

namespace dawn
{
struct LanguageDef
{
    Set<String> keywords;
    Set<String> types;
    Set<String> operators;
    String separator_identifier;
    String separator_number;
    String literal_char;
    String literal_string;
    String comment_line;
    Pair<String, String> comment_multiline;

    static LanguageDef dawn();
};
}

namespace dawn
{
struct Lexer
{
    LanguageDef lang_def = LanguageDef::dawn();

    Opt<LexError> tokenize( StringRef const& source, Array<Token>& tokens );

private:
    Bool is_space( StringRef const& source, Int i );
    Opt<LexError> extract_space( StringRef const& source, Array<Token>& tokens, Int& line, Int& i );

    Bool is_comment( StringRef const& source, Int i );
    Opt<LexError> extract_comment( StringRef const& source, Array<Token>& tokens, Int& line, Int& i );

    Bool is_mlcomment( StringRef const& source, Int i );
    Opt<LexError> extract_mlcomment( StringRef const& source, Array<Token>& tokens, Int& line, Int& i );

    Bool is_word( StringRef const& source, Int i );
    Opt<LexError> extract_word( StringRef const& source, Array<Token>& tokens, Int& line, Int& i );

    Bool is_number( StringRef const& source, Int i );
    Opt<LexError> extract_number( StringRef const& source, Array<Token>& tokens, Int& line, Int& i );

    Bool is_char( StringRef const& source, Int i );
    Opt<LexError> extract_char( StringRef const& source, Array<Token>& tokens, Int& line, Int& i );

    Bool is_string( StringRef const& source, Int i );
    Opt<LexError> extract_string( StringRef const& source, Array<Token>& tokens, Int& line, Int& i );

    Bool is_operator( StringRef const& source, Int i );
    Opt<LexError> extract_operator( StringRef const& source, Array<Token>& tokens, Int& line, Int& i );
};
}
