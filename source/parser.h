#pragma once

#include "lexer.h"
#include "node.h"


namespace dawn
{
struct Module
{
    StringSet imports;
    Vector<Variable> variables;
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
    void parse_global_variable( TokenIterator& it, Module& module );

    void parse_struct( TokenIterator& it, Struct& struc );
    void parse_enum( TokenIterator& it, Enum& en );
    void parse_function( TokenIterator& it, Function& function );
    void parse_cast( TokenIterator& it, Function& function );
    void parse_operator( TokenIterator& it, Function& operat );
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

void create_unary_node( Token const& token, Node& node );
void create_operator_node( Token const& token, Node& node );
void create_assign_node( Token const& token, Node& node );
}
