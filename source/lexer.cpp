#include "lexer.h"


std::wostream& dawn::operator<<( std::wostream& stream, LexError const& error )
{
    stream << error.msg;
    return stream;
}

std::wostream& dawn::operator<<( std::wostream& stream, TokenType type )
{
    switch ( type )
    {
    case TokenType::INTEGER: stream << L"Integer"; break;
    case TokenType::FLOAT: stream << L"Float"; break;
    case TokenType::CHAR: stream << L"Char"; break;
    case TokenType::STRING: stream << L"String"; break;
    case TokenType::KEYWORD: stream << L"Keyword"; break;
    case TokenType::TYPE: stream << L"Type"; break;
    case TokenType::FUNCTION: stream << L"Function"; break;
    case TokenType::NAME: stream << L"Name"; break;
    case TokenType::OPERATOR: stream << L"Operator"; break;
    }
    return stream;
}

std::wostream& dawn::operator<<( std::wostream& stream, Token const& token )
{
    Color color = to_color( token.type );
    stream << L"[(" << ColoredText{ color, token.type } <<
        L") {" << ColoredText{ color, token.value } <<
        L"} <" << ColoredText{ color, token.line_number } << L">]";
    return stream;
}

dawn::LanguageDef dawn::LanguageDef::dawn()
{
    LanguageDef result;
    result.keywords = {
        (String) kw_module,
        (String) kw_import,
        (String) kw_func,
        (String) kw_oper,
        (String) kw_return,
        (String) kw_yield,
        (String) kw_let,
        (String) kw_var,
        (String) kw_if,
        (String) kw_else,
        (String) kw_elif,
        (String) kw_switch,
        (String) kw_case,
        (String) kw_default,
        (String) kw_for,
        (String) kw_while,
        (String) kw_loop,
        (String) kw_continue,
        (String) kw_break,
        (String) kw_enum,
        (String) kw_layer,
        (String) kw_struct,
        (String) kw_impl,
        (String) kw_self,
        (String) kw_true,
        (String) kw_false,
        (String) kw_null,
    };
    result.types = {
        (String) tp_bool,
        (String) tp_int,
        (String) tp_float,
        (String) tp_char,
        (String) tp_string,
    };
    result.operators = {
        (String) op_add,
        (String) op_sub,
        (String) op_mul,
        (String) op_div,
        (String) op_pow,
        (String) op_mod,
        (String) op_addas,
        (String) op_subas,
        (String) op_mulas,
        (String) op_divas,
        (String) op_powas,
        (String) op_modas,
        (String) op_not,
        (String) op_and,
        (String) op_or,
        (String) op_eq,
        (String) op_neq,
        (String) op_less,
        (String) op_great,
        (String) op_lesseq,
        (String) op_greateq,
        (String) op_assign,
        (String) op_split,
        (String) op_ref,
        (String) op_access,
        (String) op_link,
        (String) op_range,
        (String) op_unknown,
        (String) op_expr_opn,
        (String) op_expr_cls,
        (String) op_scope_opn,
        (String) op_scope_cls,
        (String) op_array_opn,
        (String) op_array_cls,
        (String) op_yield_opn,
        (String) op_yield_cls,
        (String) op_expr_end,
    };
    result.separator_identifier = sep_identifier;
    result.separator_number = sep_number;
    result.literal_char = lit_char;
    result.literal_string = lit_string;
    result.comment_line = comm_line;
    result.comment_multiline = comm_multiline;
    return result;
}

dawn::Opt<dawn::LexError> dawn::Lexer::tokenize( StringRef const& source, Array<Token>& tokens )
{
    Int line = 1;
    for ( Int i = 0; i < (Int) source.size(); i++ )
    {
        if ( is_space( source, i ) )
        {
            if ( auto error = extract_space( source, tokens, line, i ) )
                return error;
        }
        else if ( is_comment( source, i ) )
        {
            if ( auto error = extract_comment( source, tokens, line, i ) )
                return error;
        }
        else if ( is_mlcomment( source, i ) )
        {
            if ( auto error = extract_mlcomment( source, tokens, line, i ) )
                return error;
        }
        else if ( is_word( source, i ) )
        {
            if ( auto error = extract_word( source, tokens, line, i ) )
                return error;
        }
        else if ( is_number( source, i ) )
        {
            if ( auto error = extract_number( source, tokens, line, i ) )
                return error;
        }
        else if ( is_char( source, i ) )
        {
            if ( auto error = extract_char( source, tokens, line, i ) )
                return error;;
        }
        else if ( is_string( source, i ) )
        {
            if ( auto error = extract_string( source, tokens, line, i ) )
                return error;;
        }
        else if ( is_operator( source, i ) )
        {
            if ( auto error = extract_operator( source, tokens, line, i ) )
                return error;
        }
        else
        {
            return LexError{ line, source[i], L"unexpected character" };
        }
    }

    return std::nullopt;
}

dawn::Bool dawn::Lexer::is_space( StringRef const& source, Int i )
{
    return iswspace( source[i] );
}

dawn::Opt<dawn::LexError> dawn::Lexer::extract_space( StringRef const& source, Array<Token>& tokens, Int& line, Int& i )
{
    for ( ; i < (Int) source.size(); i++ )
    {
        if ( source[i] == L'\n' )
            ++line;

        if ( !is_space( source, i ) )
        {
            --i;
            break;
        }
    }

    return std::nullopt;
}

dawn::Bool dawn::Lexer::is_comment( StringRef const& source, Int i )
{
    return source.substr( i ).starts_with( lang_def.comment_line );
}

dawn::Opt<dawn::LexError> dawn::Lexer::extract_comment( StringRef const& source, Array<Token>& tokens, Int& line, Int& i )
{
    for ( ; i < (Int) source.size(); i++ )
    {
        if ( source[i] == L'\n' )
        {
            ++line;
            break;
        }
    }

    return std::nullopt;
}

dawn::Bool dawn::Lexer::is_mlcomment( StringRef const& source, Int i )
{
    return source.substr( i ).starts_with( lang_def.comment_multiline.first );
}

dawn::Opt<dawn::LexError> dawn::Lexer::extract_mlcomment( StringRef const& source, Array<Token>& tokens, Int& line, Int& i )
{
    for ( ; i < (Int) source.size(); i++ )
    {
        if ( source[i] == L'\n' )
            ++line;

        if ( source.substr( i ).starts_with( lang_def.comment_multiline.second ) )
        {
            i += lang_def.comment_multiline.second.size() - 1;
            break;
        }
    }

    return std::nullopt;
}

dawn::Bool dawn::Lexer::is_word( StringRef const& source, Int i )
{
    return source.substr( i ).starts_with( lang_def.separator_identifier ) || iswalpha( source[i] );
}

dawn::Opt<dawn::LexError> dawn::Lexer::extract_word( StringRef const& source, Array<Token>& tokens, Int& line, Int& i )
{
    String buffer;
    for ( ; i < (Int) source.size(); i++ )
    {
        if ( !iswdigit( source[i] ) && !is_word( source, i ) )
        {
            --i;
            break;
        }
        buffer.push_back( source[i] );
    }

    TokenType type;
    if ( lang_def.keywords.contains( buffer ) )
    {
        type = TokenType::KEYWORD;
    }
    else if ( iswupper( buffer.front() ) || lang_def.types.contains( buffer ) )
    {
        type = TokenType::TYPE;
    }
    else
    {
        type = TokenType::NAME;
    }

    Token& token = tokens.emplace_back();
    token.type = type;
    token.value = buffer;
    token.line_number = line;

    return std::nullopt;
}

dawn::Bool dawn::Lexer::is_number( StringRef const& source, Int i )
{
    return source.substr( i ).starts_with( lang_def.separator_number ) || iswdigit( source[i] );
}

dawn::Opt<dawn::LexError> dawn::Lexer::extract_number( StringRef const& source, Array<Token>& tokens, Int& line, Int& i )
{
    String buffer;
    Bool is_float = false;
    for ( ; i < (Int) source.size(); i++ )
    {
        if ( source.substr( i ).starts_with( lang_def.separator_number ) )
        {
            if ( is_float )
                return LexError{ line, source[i], L"invalid float number" };

            is_float = true;
        }
        else if ( !is_number( source, i ) )
        {
            --i;
            break;
        }
        buffer.push_back( source[i] );
    }

    if ( buffer == lang_def.separator_number )
        return LexError{ line, source[i], L"invalid number" };

    Token& token = tokens.emplace_back();
    token.type = is_float ? TokenType::FLOAT : TokenType::INTEGER;
    token.value = buffer;
    token.line_number = line;

    return std::nullopt;
}

dawn::Bool dawn::Lexer::is_char( StringRef const& source, Int i )
{
    return source.substr( i ).starts_with( lang_def.literal_char );
}

dawn::Opt<dawn::LexError> dawn::Lexer::extract_char( StringRef const& source, Array<Token>& tokens, Int& line, Int& i )
{
    if ( source.substr( i ).size() < 3 )
        return LexError{ line, source[i], L"char literal too short" };

    String buffer;
    if ( source[i + 1] == L'\\' )
    {
        if ( source.substr( i ).size() < 4 )
            return LexError{ line, source[i], L"escaping char too short" };

        if ( !is_char( source, i + 3 ) )
            return LexError{ line, source[i], L"invalid escaping char literal" };

        Char c = to_escaping( source[i + 2] );
        buffer = String( 1, c );
        i += 3;
    }
    else
    {
        if ( !is_char( source, i + 2 ) )
            return LexError{ line, source[i], L"invalid char literal" };

        Char c = source[i + 1];
        buffer = String( 1, c );
        i += 2;
    }

    Token& token = tokens.emplace_back();
    token.type = TokenType::CHAR;
    token.value = buffer;
    token.line_number = line;

    return std::nullopt;
}

dawn::Bool dawn::Lexer::is_string( StringRef const& source, Int i )
{
    return source.substr( i ).starts_with( lang_def.literal_string );
}

dawn::Opt<dawn::LexError> dawn::Lexer::extract_string( StringRef const& source, Array<Token>& tokens, Int& line, Int& i )
{
    String buffer;
    i += lang_def.literal_string.size();
    for ( ; i < (Int) source.size(); i++ )
    {
        if ( source[i] == L'\n' )
            ++line;

        if ( source.substr( i ).starts_with( lang_def.literal_string ) )
        {
            i += lang_def.literal_string.size() - 1;
            break;
        }

        if ( source[i] == L'\\' )
        {
            auto view = source.substr( i );
            if ( view.size() < 2 )
                return LexError{ line, source[i], L"string escaping char too short" };

            Char c = to_escaping( view[1] );
            buffer.push_back( c );
            ++i;
        }
        else
        {
            buffer.push_back( source[i] );
        }
    }

    Token& token = tokens.emplace_back();
    token.type = TokenType::STRING;
    token.value = buffer;
    token.line_number = line;

    return std::nullopt;
}

dawn::Bool dawn::Lexer::is_operator( StringRef const& source, Int i )
{
    for ( auto& op : lang_def.operators )
    {
        if ( source.substr( i ).starts_with( op ) )
            return true;
    }
    return false;
}

dawn::Opt<dawn::LexError> dawn::Lexer::extract_operator( StringRef const& source, Array<Token>& tokens, Int& line, Int& i )
{
    Int op_size = 0;
    Opt<String> closest_op;
    for ( auto& op : lang_def.operators )
    {
        if ( source.substr( i ).starts_with( op ) && (Int) op.size() > op_size )
        {
            op_size = op.size();
            closest_op.emplace( op );
        }
    }
    i += op_size - 1;

    if ( !closest_op )
        return LexError{ line, source[i], L"unknown operator" };

    Token& token = tokens.emplace_back();
    token.type = TokenType::OPERATOR;
    token.value = closest_op.value();
    token.line_number = line;

    return std::nullopt;
}
