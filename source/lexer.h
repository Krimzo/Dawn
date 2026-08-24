#pragma once

#include "syntax.h"
#include "err.h"

namespace dawn
{
inline const ID id_void = tp_void;
inline const ID id_bool = tp_bool;
inline const ID id_int = tp_int;
inline const ID id_float = tp_float;
inline const ID id_char = tp_char;
inline const ID id_string = tp_string;
inline const ID id_range = tp_range;
inline const ID id_function = tp_function;
inline const ID id_array = tp_array;

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

    static Source from_text(StringRef const& str);
    static Source from_file(StringRef const& path);

    constexpr Char operator[](Int i) const
    {
        return source[i];
    }

    constexpr StringRef substr(Int i) const
    {
        return StringRef{source}.substr(i);
    }

    constexpr Int size() const
    {
        return (Int)source.size();
    }

  private:
    explicit Source(Opt<String> path, String source) : path(std::move(path)), source(std::move(source))
    {
    }
};

struct Lexer
{
    LanguageDef lang_def = LanguageDef::dawn();

    void tokenize(Source const& source, Vector<Token>& tokens) const;
    void tokenize_at(Source const& source, Vector<Token>& tokens, Index& index) const;

  private:
    Bool is_space(Source const& source, Int i) const;
    void extract_space(Source const& source, Vector<Token>& tokens, Index& index) const;

    Bool is_comment(Source const& source, Int i) const;
    void extract_comment(Source const& source, Vector<Token>& tokens, Index& index) const;

    Bool is_mlcomment(Source const& source, Int i) const;
    void extract_mlcomment(Source const& source, Vector<Token>& tokens, Index& index) const;

    Bool is_word(Source const& source, Int i) const;
    void extract_word(Source const& source, Vector<Token>& tokens, Index& index) const;

    Bool is_number(Source const& source, Int i) const;
    void extract_number(Source const& source, Vector<Token>& tokens, Index& index) const;

    Bool is_char(Source const& source, Int i) const;
    void extract_char(Source const& source, Vector<Token>& tokens, Index& index) const;

    Bool is_string(Source const& source, Int i) const;
    void extract_string(Source const& source, Vector<Token>& tokens, Index& index) const;

    Bool is_operator(Source const& source, Int i) const;
    void extract_operator(Source const& source, Vector<Token>& tokens, Index& index) const;
};
} // namespace dawn
