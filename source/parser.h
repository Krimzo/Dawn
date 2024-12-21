#pragma once

#include "lexer.h"
#include "nodes.h"


namespace dawn
{
enum struct ExtractType
{
    DEFAULT,
    NEW_LINE,
    SPLITTER,
    SCOPE_START,
};

struct Module
{
    Set<String> imports;
    Array<Variable> variables;
    Array<Function> functions;
    Array<Enum> enums;
    Array<Struct> structs;

    Bool contains_id( Int id ) const;
};

struct Parser
{
    void parse( Array<Token>& tokens, Module& module );

private:
    void parse_import( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module );
    void parse_global_struct( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module );
    void parse_global_enum( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module );
    void parse_global_function( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module );
    void parse_global_variable( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module );

    void parse_struct( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Struct& struc );
    void parse_enum( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Enum& enu );
    void parse_function( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Function& function );
    void parse_operator( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Function& operat );
    void parse_variable( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Variable& variable );

    void parse_expression( ExtractType type, Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Node& tree );
    void expression_extract( ExtractType type, Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Array<Token>& tokens );
    void expression_precedence( Array<Token>& tokens, Int& index, Bool& unary );
    void expression_complex_expr( Array<Token>& left, Array<Token>& right, Node& tree );
    void expression_complex_scope( Array<Token>& left, Array<Token>& right, Node& tree );
    void expression_complex_array( Array<Token>& left, Array<Token>& right, Node& tree );
    void expression_complex_default( Array<Token>& left, Token op, Array<Token>& right, Node& tree );
    void expression_pure( Array<Token>& tokens, Node& tree );
    void expression_single( Token const& token, Node& tree );
    void expression_single_literal( Token const& token, Node& tree );
    void expression_single_keyword( Token const& token, Node& tree );
    void expression_single_identifier( Token const& token, Node& tree );

    void parse_scope( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Scope& scope );
    void scope_return( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Node& tree );
    void scope_break( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Node& tree );
    void scope_continue( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Node& tree );
    void scope_throw( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Node& tree );
    void scope_try( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Node& tree );
    void scope_if( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Node& tree );
    void scope_switch( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Node& tree );
    void scope_loop( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Node& tree );
    void scope_while( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Node& tree );
    void scope_for( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Node& tree );
};

Bool is_unary( Token const& token );
Int token_depth( Token const& token, Bool& in_lambda );

void create_unary_node( Token const& token, Node& node );
void create_operator_node( Token const& token, Node& node );
void create_assign_node( Token const& token, Node& node );

Node make_def_val( StringRef const& type );
}
