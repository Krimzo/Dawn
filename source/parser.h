#pragma once

#include "lexer.h"
#include "node.h"


namespace dawn
{
struct Module
{
    StringSet imports;
    Vector<Variable> variables;
    Vector<Operator> operators;
    Vector<Function> functions;
    Vector<Enum> enums;
    Vector<Struct> structs;

    Bool contains_id( ID id ) const;
};

struct TokenIterator
{
    constexpr TokenIterator( Token const* ptr, Token const* end )
        : m_ptr( ptr ), m_start( ptr ), m_end( end )
    {
    }

    Bool valid() const;
    Token const& operator*() const;
    Token const* operator->() const;
    Bool operator==( TokenIterator const& other ) const;
    void operator++();
    void operator--();

private:
    Token const* m_ptr = nullptr;
    Token const* const m_start;
    Token const* const m_end;
};

struct Parser
{
    void parse( Vector<Token> const& tokens, Module& module );
    void parse( Token const* token_ptr, Int token_count, Module& module );

private:
    Bool is_variable( TokenIterator const& it );

    void parse_import( TokenIterator& it, Module& module );
    void parse_global_struct( TokenIterator& it, Module& module );
    void parse_global_enum( TokenIterator& it, Module& module );
    void parse_global_function( TokenIterator& it, Module& module );
    void parse_global_operator( TokenIterator& it, Module& module );
    void parse_global_variable( TokenIterator& it, Module& module );

    void parse_struct( TokenIterator& it, Struct& struc );
    void parse_enum( TokenIterator& it, Enum& en );
    void parse_operator( TokenIterator& it, Operator& oper );
    void parse_function( TokenIterator& it, Function& function );
    void parse_cast( TokenIterator& it, Function& function );
    void parse_variable( TokenIterator& it, Variable& variable );

    void parse_expression( ExtractType type, TokenIterator& it, Node& tree );
    void expression_extract( ExtractType type, TokenIterator& it, Vector<Token>& tokens );
    void expression_precedence( Vector<Token>& tokens, Int& index, Bool& unary );
    void expression_complex_expr( Vector<Token>& left, Token op, Vector<Token>& right, Node& tree );
    void expression_complex_scope( Vector<Token>& left, Token op, Vector<Token>& right, Node& tree );
    void expression_complex_array( Vector<Token>& left, Token op, Vector<Token>& right, Node& tree );
    void expression_complex_link( Vector<Token>& left, Token op, Vector<Token>& right, Node& tree );
    void expression_complex_default( Vector<Token>& left, Token op, Vector<Token>& right, Node& tree );
    void expression_pure( Vector<Token>& tokens, Node& tree );
    void expression_single( Token const& token, Node& tree );
    void expression_single_literal( Token const& token, Node& tree );
    void expression_single_keyword( Token const& token, Node& tree );
    void expression_single_type( Token const& token, Node& tree );
    void expression_single_identifier( Token const& token, Node& tree );

    void parse_scope( TokenIterator& it, Scope& scope );
    void scope_return( TokenIterator& it, Node& tree );
    void scope_break( TokenIterator& it, Node& tree );
    void scope_continue( TokenIterator& it, Node& tree );
    void scope_throw( TokenIterator& it, Node& tree );
    void scope_try( TokenIterator& it, Node& tree );
    void scope_if( TokenIterator& it, Node& tree );
    void scope_switch( TokenIterator& it, Node& tree );
    void scope_loop( TokenIterator& it, Node& tree );
    void scope_while( TokenIterator& it, Node& tree );
    void scope_for( TokenIterator& it, Node& tree );
};

Bool is_unary( Token const& token );
Int token_depth( Token const& token, Bool& in_lambda );

void create_operator_node( Token const& token, Node& node );
void create_assign_node( Token const& token, Node& node );

__forceinline Bool is_op( StringRef const& value )
{
    static const StringSet OPS = {
        (String) op_add,
        (String) op_sub,
        (String) op_mul,
        (String) op_div,
        (String) op_pow,
        (String) op_mod,
        (String) op_eq,
        (String) op_neq,
        (String) op_less,
        (String) op_great,
        (String) op_lesseq,
        (String) op_greateq,
        (String) op_not,
        (String) op_and,
        (String) op_or,
        (String) op_range,
    };
    return OPS.contains( value );
}

__forceinline OperatorType get_op( StringRef const& value )
{
    if ( value == op_add )
        return OperatorType::ADD;

    else if ( value == op_sub )
        return OperatorType::SUB;

    else if ( value == op_mul )
        return OperatorType::MUL;

    else if ( value == op_div )
        return OperatorType::DIV;

    else if ( value == op_pow )
        return OperatorType::POW;

    else if ( value == op_mod )
        return OperatorType::MOD;

    else if ( value == op_eq )
        return OperatorType::EQ;

    else if ( value == op_neq )
        return OperatorType::NOT_EQ;

    else if ( value == op_less )
        return OperatorType::LESS;

    else if ( value == op_great )
        return OperatorType::GREAT;

    else if ( value == op_lesseq )
        return OperatorType::LESS_EQ;

    else if ( value == op_greateq )
        return OperatorType::GREAT_EQ;

    else if ( value == op_not )
        return OperatorType::NOT;

    else if ( value == op_and )
        return OperatorType::AND;

    else if ( value == op_or )
        return OperatorType::OR;

    else if ( value == op_range )
        return OperatorType::RANGE;

    else
        PARSER_PANIC( {}, "unknown operator [", value, "]" );
}
}
