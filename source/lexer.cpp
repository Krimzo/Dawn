#include "lexer.h"


dawn::LanguageDef dawn::LanguageDef::dawn()
{
    LanguageDef result;
    result.keywords = {
        (String) kw_import,
        (String) kw_func,
        (String) kw_cast,
        (String) kw_oper,
        (String) kw_return,
        (String) kw_let,
        (String) kw_var,
        (String) kw_ref,
        (String) kw_if,
        (String) kw_elif,
        (String) kw_else,
        (String) kw_switch,
        (String) kw_case,
        (String) kw_default,
        (String) kw_for,
        (String) kw_while,
        (String) kw_loop,
        (String) kw_continue,
        (String) kw_break,
        (String) kw_throw,
        (String) kw_try,
        (String) kw_catch,
        (String) kw_enum,
        (String) kw_struct,
        (String) kw_self,
        (String) kw_true,
        (String) kw_false,
    };
    result.types = {
        (String) tp_nothing,
        (String) tp_bool,
        (String) tp_int,
        (String) tp_float,
        (String) tp_char,
        (String) tp_string,
        (String) tp_function,
        (String) tp_array,
        (String) tp_range,
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
        (String) op_cmpr,
        (String) op_eq,
        (String) op_neq,
        (String) op_less,
        (String) op_great,
        (String) op_lesseq,
        (String) op_greateq,
        (String) op_assign,
        (String) op_link,
        (String) op_access,
        (String) op_range,
        (String) op_set,
        (String) op_split,
        (String) op_lambda,
        (String) op_expr_opn,
        (String) op_expr_cls,
        (String) op_scope_opn,
        (String) op_scope_cls,
        (String) op_array_opn,
        (String) op_array_cls,
    };
    result.separator_identifier = sep_identifier;
    result.separator_number = sep_number;
    result.literal_char = lit_char;
    result.literal_string = lit_string;
    result.comment_line = comm_line;
    result.comment_multiline = comm_multiline;
    result.cmplx_string_opn = op_scope_opn;
    result.cmplx_string_cls = op_scope_cls;
    result.oper_add = op_add;
    result.oper_sub = op_sub;
    result.expo_number = exp_number;
    result.to_string = tp_string;
    result.call_opn = op_expr_opn;
    result.call_cls = op_expr_cls;
    result.expr_opn = op_expr_opn;
    result.expr_cls = op_expr_cls;
    return result;
}

void dawn::Lexer::tokenize( Source const& source, Vector<Token>& tokens )
{
    for ( Index index; index.index() < source.size(); index.incr() )
        tokenize_at( source, tokens, index );
}

void dawn::Lexer::tokenize_at( Source const& source, Vector<Token>& tokens, Index& index )
{
    if ( is_space( source, index.index() ) )
    {
        extract_space( source, tokens, index );
    }
    else if ( is_comment( source, index.index() ) )
    {
        extract_comment( source, tokens, index );
    }
    else if ( is_mlcomment( source, index.index() ) )
    {
        extract_mlcomment( source, tokens, index );
    }
    else if ( is_word( source, index.index() ) )
    {
        extract_word( source, tokens, index );
    }
    else if ( is_number( source, index.index() ) )
    {
        extract_number( source, tokens, index );
    }
    else if ( is_char( source, index.index() ) )
    {
        extract_char( source, tokens, index );
    }
    else if ( is_string( source, index.index() ) )
    {
        extract_string( source, tokens, index );
    }
    else if ( is_operator( source, index.index() ) )
    {
        extract_operator( source, tokens, index );
    }
    else
        LEXER_PANIC( Location{ source.path.value_or( {} ), index }, source[index.index()], "unexpected character" );
}

dawn::Bool dawn::Lexer::is_space( Source const& source, Int i )
{
    return isspace( source[i] );
}

void dawn::Lexer::extract_space( Source const& source, Vector<Token>& tokens, Index& index )
{
    for ( ; index.index() < source.size(); index.incr() )
    {
        if ( source[index.index()] == '\n' )
            index.new_line();

        if ( !is_space( source, index.index() ) )
        {
            index.decr();
            break;
        }
    }
}

dawn::Bool dawn::Lexer::is_comment( Source const& source, Int i )
{
    return source.substr( i ).starts_with( lang_def.comment_line );
}

void dawn::Lexer::extract_comment( Source const& source, Vector<Token>& tokens, Index& index )
{
    for ( ; index.index() < source.size(); index.incr() )
    {
        if ( source[index.index()] == '\n' )
        {
            index.new_line();
            break;
        }
    }
}

dawn::Bool dawn::Lexer::is_mlcomment( Source const& source, Int i )
{
    return source.substr( i ).starts_with( lang_def.comment_multiline.first );
}

void dawn::Lexer::extract_mlcomment( Source const& source, Vector<Token>& tokens, Index& index )
{
    for ( ; index.index() < source.size(); index.incr() )
    {
        if ( source[index.index()] == '\n' )
            index.new_line();

        if ( source.substr( index.index() ).starts_with( lang_def.comment_multiline.second ) )
        {
            index.incr( lang_def.comment_multiline.second.size() - 1 );
            break;
        }
    }
}

dawn::Bool dawn::Lexer::is_word( Source const& source, Int i )
{
    return source.substr( i ).starts_with( lang_def.separator_identifier ) || isalpha( source[i] );
}

void dawn::Lexer::extract_word( Source const& source, Vector<Token>& tokens, Index& index )
{
    String buffer;
    for ( ; index.index() < source.size(); index.incr() )
    {
        if ( !isdigit( source[index.index()] ) && !is_word( source, index.index() ) )
        {
            index.decr();
            break;
        }
        buffer.push_back( source[index.index()] );
    }

    TokenType type;
    if ( lang_def.keywords.contains( buffer ) )
        type = TokenType::KEYWORD;
    else if ( is_custom_type( buffer ) || lang_def.types.contains( buffer ) )
        type = TokenType::TYPE;
    else
        type = TokenType::NAME;

    auto& token = tokens.emplace_back();
    token.type = type;
    token.value = buffer;
    token.location = Location{ source.path.value_or( {} ), index };
}

dawn::Bool dawn::Lexer::is_number( Source const& source, Int i )
{
    return isdigit( source[i] ) ||
        ( source.substr( i ).starts_with( lang_def.separator_number ) && source.size() > ( i + 1 ) && isdigit( source[i + 1] ) );
}

void dawn::Lexer::extract_number( Source const& source, Vector<Token>& tokens, Index& index )
{
    String buffer;
    Bool is_float = false, is_scientific = false;
    for ( ; index.index() < source.size(); index.incr() )
    {
        if ( source.substr( index.index() ).starts_with( lang_def.separator_number ) )
        {
            if ( is_scientific )
                LEXER_PANIC( Location{ source.path.value_or( {} ), index }, source[index.index()], "exponent value must be an integer" );
            if ( is_float )
                LEXER_PANIC( Location{ source.path.value_or( {} ), index }, source[index.index()], "float number can only contain a single number separator" );
            is_float = true;
        }
        else if ( source.substr( index.index() ).starts_with( lang_def.expo_number ) )
        {
            if ( is_scientific )
                LEXER_PANIC( Location{ source.path.value_or( {} ), index }, source[index.index()], "scientific form number can only contain a single exponent" );
            is_scientific = true;
        }
        else if ( is_scientific && ( source.substr( index.index() ).starts_with( lang_def.oper_add ) || source.substr( index.index() ).starts_with( lang_def.oper_sub ) ) )
        {
            if ( &buffer.back() != lang_def.expo_number )
                LEXER_PANIC( Location{ source.path.value_or( {} ), index }, source[index.index()], "sign can only be applied to the exponent" );
        }
        else if ( !is_number( source, index.index() ) )
        {
            index.decr();
            break;
        }
        buffer.push_back( source[index.index()] );
    }

    if ( &buffer.back() == lang_def.expo_number
        || &buffer.back() == lang_def.oper_add
        || &buffer.back() == lang_def.oper_sub )
        LEXER_PANIC( Location{ source.path.value_or( {} ), index }, source[index.index()], "expected an exponent" );

    auto& token = tokens.emplace_back();
    token.type = ( is_float || is_scientific ) ? TokenType::FLOAT : TokenType::INTEGER;
    token.literal = buffer;
    token.location = Location{ source.path.value_or( {} ), index };
}

dawn::Bool dawn::Lexer::is_char( Source const& source, Int i )
{
    return source.substr( i ).starts_with( lang_def.literal_char );
}

void dawn::Lexer::extract_char( Source const& source, Vector<Token>& tokens, Index& index )
{
    if ( source.substr( index.index() ).size() < 3 )
        LEXER_PANIC( Location{ source.path.value_or( {} ), index }, source[index.index()], "char literal too short" );

    String buffer;
    if ( source[index.index() + 1] == '\\' )
    {
        if ( source.substr( index.index() ).size() < 4 )
            LEXER_PANIC( Location{ source.path.value_or( {} ), index }, source[index.index()], "escaping char too short" );

        if ( !is_char( source, index.index() + 3 ) )
            LEXER_PANIC( Location{ source.path.value_or( {} ), index }, source[index.index()], "invalid escaping char literal" );

        Char c = to_escaping( source[index.index() + 2] );
        buffer = String{ c };
        index.incr( 3 );
    }
    else
    {
        if ( !is_char( source, index.index() + 2 ) )
            LEXER_PANIC( Location{ source.path.value_or( {} ), index }, source[index.index()], "invalid char literal" );

        Char c = source[index.index() + 1];
        buffer = String{ c };
        index.incr( 2 );
    }

    auto& token = tokens.emplace_back();
    token.type = TokenType::CHAR;
    token.literal = buffer;
    token.location = Location{ source.path.value_or( {} ), index };
}

dawn::Bool dawn::Lexer::is_string( Source const& source, Int i )
{
    return source.substr( i ).starts_with( lang_def.literal_string );
}

void dawn::Lexer::extract_string( Source const& source, Vector<Token>& tokens, Index& index )
{
    const auto add_value_token = [&]( TokenType type, StringRef const& str )
        {
            auto& token = tokens.emplace_back();
            token.type = type;
            token.value = str;
            token.location = Location{ source.path.value_or( {} ), index };
        };
    const auto add_literal_token = [&]( TokenType type, StringRef const& str )
        {
            auto& token = tokens.emplace_back();
            token.type = type;
            token.literal = str;
            token.location = Location{ source.path.value_or( {} ), index };
        };

    add_value_token( TokenType::OPERATOR, lang_def.expr_opn );
    const Int open_expr_indx = (Int) tokens.size() - 1;

    String buffer;
    index.incr( lang_def.literal_string.size() );
    Int cmplx_part_count = 0;
    for ( ; index.index() < source.size(); index.incr() )
    {
        if ( source[index.index()] == '\n' )
            index.new_line();

        if ( source.substr( index.index() ).starts_with( lang_def.literal_string ) )
        {
            index.incr( lang_def.literal_string.size() - 1 );
            break;
        }

        if ( source[index.index()] == '\\' )
        {
            auto view = source.substr( index.index() );
            if ( view.size() < 2 )
                LEXER_PANIC( Location{ source.path.value_or( {} ), index }, source[index.index()], "string escaping char too short" );

            Char c = to_escaping( view[1] );
            buffer.push_back( c );
            index.incr();
        }
        else if ( source.substr( index.index() ).starts_with( lang_def.cmplx_string_opn ) )
        {
            if ( !buffer.empty() )
            {
                add_literal_token( TokenType::STRING, buffer );
                buffer.clear();
                add_value_token( TokenType::OPERATOR, lang_def.oper_add );
                cmplx_part_count += 1;
            }

            add_value_token( TokenType::NAME, lang_def.to_string );
            add_value_token( TokenType::OPERATOR, lang_def.call_opn );

            index.incr( lang_def.cmplx_string_opn.size() );
            Int cmplx_str_depth = 1;
            for ( ; index.index() < source.size(); index.incr() )
            {
                if ( source.substr( index.index() ).starts_with( lang_def.cmplx_string_opn ) )
                    cmplx_str_depth += 1;
                else if ( source.substr( index.index() ).starts_with( lang_def.cmplx_string_cls ) )
                {
                    cmplx_str_depth -= 1;
                    if ( cmplx_str_depth == 0 )
                        break;
                }
                tokenize_at( source, tokens, index );
            }

            add_value_token( TokenType::OPERATOR, lang_def.call_cls );
            add_value_token( TokenType::OPERATOR, lang_def.oper_add );
            cmplx_part_count += 1;
        }
        else
            buffer.push_back( source[index.index()] );
    }

    if ( cmplx_part_count > 0 )
    {
        if ( buffer.empty() )
            tokens.pop_back(); // Remove oper_add.
        else
        {
            add_literal_token( TokenType::STRING, buffer );
            cmplx_part_count += 1;
        }

        if ( cmplx_part_count > 1 )
            add_value_token( TokenType::OPERATOR, lang_def.expr_cls );
        else
            tokens.erase( tokens.begin() + open_expr_indx ); // Remove expr_opn.
    }
    else
    {
        tokens.erase( tokens.begin() + open_expr_indx ); // Remove expr_opn.
        add_literal_token( TokenType::STRING, buffer );
    }
}

dawn::Bool dawn::Lexer::is_operator( Source const& source, Int i )
{
    for ( auto& op : lang_def.operators )
    {
        if ( source.substr( i ).starts_with( op ) )
            return true;
    }
    return false;
}

void dawn::Lexer::extract_operator( Source const& source, Vector<Token>& tokens, Index& index )
{
    Int op_size = 0;
    Opt<String> closest_op;
    for ( auto& op : lang_def.operators )
    {
        if ( source.substr( index.index() ).starts_with( op ) && (Int) op.size() > op_size )
        {
            op_size = op.size();
            closest_op.emplace( op );
        }
    }
    index.incr( op_size - 1 );

    if ( !closest_op )
        LEXER_PANIC( Location{ source.path.value_or( {} ), index }, source[index.index()], "unknown operator" );

    auto& token = tokens.emplace_back();
    token.type = TokenType::OPERATOR;
    token.value = *closest_op;
    token.location = Location{ source.path.value_or( {} ), index };
}
