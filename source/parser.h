#pragma once

#include "lexer.h"
#include "node.h"


namespace dawn
{
struct Module
{
    Set<String> imports;
    Vector<Variable> variables;
    Vector<Function> functions;
    Vector<Enum> enums;
    Vector<Struct> structs;

    Bool contains_id( Int id ) const;
};

struct Parser
{
    void parse( Vector<Token>& tokens, Module& module );

private:
    void parse_import( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Module& module );
    void parse_global_struct( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Module& module );
    void parse_global_enum( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Module& module );
    void parse_global_function( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Module& module );
    void parse_global_variable( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Module& module );

    void parse_struct( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Struct& struc );
    void parse_enum( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Enum& en );
    void parse_function( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Function& function );
    void parse_cast( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Function& function );
    void parse_operator( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Function& operat );
    void parse_variable( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Variable& variable );

    void parse_expression( ExtractType type, Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Node& tree );
    void expression_extract( ExtractType type, Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Vector<Token>& tokens );
    void expression_precedence( Vector<Token>& tokens, Int& index, Bool& unary );
    void expression_complex_expr( Vector<Token>& left, Token op, Vector<Token>& right, Node& tree );
    void expression_complex_scope( Vector<Token>& left, Token op, Vector<Token>& right, Node& tree );
    void expression_complex_array( Vector<Token>& left, Token op, Vector<Token>& right, Node& tree );
    void expression_complex_default( Vector<Token>& left, Token op, Vector<Token>& right, Node& tree );
    void expression_pure( Vector<Token>& tokens, Node& tree );
    void expression_single( Token const& token, Node& tree );
    void expression_single_literal( Token const& token, Node& tree );
    void expression_single_keyword( Token const& token, Node& tree );
    void expression_single_type( Token const& token, Node& tree );
    void expression_single_identifier( Token const& token, Node& tree );

    void parse_scope( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Scope& scope );
    void scope_return( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Node& tree );
    void scope_break( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Node& tree );
    void scope_continue( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Node& tree );
    void scope_throw( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Node& tree );
    void scope_try( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Node& tree );
    void scope_if( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Node& tree );
    void scope_switch( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Node& tree );
    void scope_loop( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Node& tree );
    void scope_while( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Node& tree );
    void scope_for( Vector<Token>::const_iterator& it, Vector<Token>::const_iterator const& end, Node& tree );
};

Bool is_unary( Token const& token );
Int token_depth( Token const& token, Bool& in_lambda );

void create_unary_node( Token const& token, Node& node );
void create_operator_node( Token const& token, Node& node );
void create_assign_node( Token const& token, Node& node );
}
