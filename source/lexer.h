#pragma once

#include "syntax.h"
#include "err.h"


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

struct Lexer
{
    LanguageDef lang_def = LanguageDef::dawn();

    void tokenize( StringRef const& source, Array<Token>& tokens );

private:
    Bool is_space( StringRef const& source, Int i );
    void extract_space( StringRef const& source, Array<Token>& tokens, Int& line, Int& i );

    Bool is_comment( StringRef const& source, Int i );
    void extract_comment( StringRef const& source, Array<Token>& tokens, Int& line, Int& i );

    Bool is_mlcomment( StringRef const& source, Int i );
    void extract_mlcomment( StringRef const& source, Array<Token>& tokens, Int& line, Int& i );

    Bool is_word( StringRef const& source, Int i );
    void extract_word( StringRef const& source, Array<Token>& tokens, Int& line, Int& i );

    Bool is_number( StringRef const& source, Int i );
    void extract_number( StringRef const& source, Array<Token>& tokens, Int& line, Int& i );

    Bool is_char( StringRef const& source, Int i );
    void extract_char( StringRef const& source, Array<Token>& tokens, Int& line, Int& i );

    Bool is_string( StringRef const& source, Int i );
    void extract_string( StringRef const& source, Array<Token>& tokens, Int& line, Int& i );

    Bool is_operator( StringRef const& source, Int i );
    void extract_operator( StringRef const& source, Array<Token>& tokens, Int& line, Int& i );
};
}
