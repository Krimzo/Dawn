#pragma once

#include "syntax.h"
#include "err.h"


namespace dawn
{
struct LanguageDef
{
    StringSet keywords;
    StringSet types;
    StringSet operators;
    String separator_identifier;
    String separator_number;
    String literal_char;
    String literal_string;
    String comment_line;
    Pair<String, String> comment_multiline;
    String cmplx_string_opn;
    String cmplx_string_cls;
    String oper_add;
    String oper_sub;
    String expo_number;
    String to_string;
    String call_opn;
    String call_cls;
    String expr_opn;
    String expr_cls;

    static LanguageDef dawn();
};

struct Source
{
    const Opt<String> path;
    const String source;

    static Source from_text( StringRef const& str )
    {
        return Source{ std::nullopt, String{ str } };
    }

    static Source from_file( StringRef const& path )
    {
        const String abs_path = fs::canonical( path ).generic_string();
        if ( const auto opt_str = read_file( abs_path ) )
            return Source{ abs_path, *opt_str };
        LEXER_PANIC( Location{}, Char{}, "failed to read file ", abs_path );
    }

    constexpr Char operator[]( Int i ) const
    {
        return source[i];
    }

    constexpr StringRef substr( Int i ) const
    {
        return StringRef{ source }.substr( i );
    }

    constexpr Int size() const
    {
        return (Int) source.size();
    }

private:
    explicit Source( Opt<String> path, String source )
        : path( std::move( path ) ), source( std::move( source ) )
    {
    }
};

struct Lexer
{
    LanguageDef lang_def = LanguageDef::dawn();

    void tokenize( Source const& source, Vector<Token>& tokens );
    void tokenize_at( Source const& source, Vector<Token>& tokens, Index& index );

private:
    Bool is_space( Source const& source, Int i );
    void extract_space( Source const& source, Vector<Token>& tokens, Index& index );

    Bool is_comment( Source const& source, Int i );
    void extract_comment( Source const& source, Vector<Token>& tokens, Index& index );

    Bool is_mlcomment( Source const& source, Int i );
    void extract_mlcomment( Source const& source, Vector<Token>& tokens, Index& index );

    Bool is_word( Source const& source, Int i );
    void extract_word( Source const& source, Vector<Token>& tokens, Index& index );

    Bool is_number( Source const& source, Int i );
    void extract_number( Source const& source, Vector<Token>& tokens, Index& index );

    Bool is_char( Source const& source, Int i );
    void extract_char( Source const& source, Vector<Token>& tokens, Index& index );

    Bool is_string( Source const& source, Int i );
    void extract_string( Source const& source, Vector<Token>& tokens, Index& index );

    Bool is_operator( Source const& source, Int i );
    void extract_operator( Source const& source, Vector<Token>& tokens, Index& index );
};
}
