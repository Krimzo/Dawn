#pragma once

#include "lexer.h"
#include "nodes.h"


namespace dawn
{
struct ParseError
{
    String msg;

    template<typename... Args>
    ParseError( Opt<Token> const& token, Args const&... args )
    {
        StringStream stream;
        if ( token )
        {
            stream << L"Parser error at token " << *token << L": ";
        }
        else
        {
            stream << L"Parser error: ";
        }
        (stream << ... << args);
        msg = stream.str();
    }

    inline operator auto& () const
    {
        return msg;
    }
};

std::wostream& operator<<( std::wostream& stream, ParseError const& error );

struct Module
{
    Array<Variable> variables;
    Array<Function> functions;
    Array<Enum> enums;
    Array<Struct> structs;

    Bool contains_id( StringRef const& id ) const;
};

struct Parser
{
    Opt<ParseError> parse( Array<Token>& tokens, Module& module );

private:
    void prepare_tokens( Array<Token>& tokens );

    Opt<ParseError> parse_global_struct( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module );
    Opt<ParseError> parse_global_enum( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module );
    Opt<ParseError> parse_global_function( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module );
    Opt<ParseError> parse_global_variable( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module );

    Opt<ParseError> parse_type( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, String& type );
    Opt<ParseError> parse_struct( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Struct& struc );
    Opt<ParseError> parse_enum( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Enum& enu );
    Opt<ParseError> parse_function( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Function& function );
    Opt<ParseError> parse_variable( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Variable& variable );

    Opt<ParseError> parse_expression( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Node>& tree );
    Opt<ParseError> expression_extract( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Array<Token>& tokens );
    Opt<ParseError> expression_precedence( Array<Token> const& tokens, Int& index );
    Opt<ParseError> expression_pure( Array<Token> const& tokens, Ref<Node>& tree );
    Opt<ParseError> expression_single( Token const& token, Ref<Node>& tree );
    Opt<ParseError> expression_single_literal( Token const& token, Ref<Node>& tree );
    Opt<ParseError> expression_single_keyword( Token const& token, Ref<Node>& tree );
    Opt<ParseError> expression_single_identifier( Token const& token, Ref<Node>& tree );
    Opt<ParseError> expression_type( Array<Token> const& tokens, Ref<Node>& tree );
    Opt<ParseError> expression_type_cast( Array<Token> const& tokens, Ref<Node>& tree );
    Opt<ParseError> expression_type_make( Array<Token> const& tokens, Ref<Node>& tree );
    Opt<ParseError> expression_type_array( Array<Token> const& tokens, Ref<Node>& tree );
    Opt<ParseError> expression_function( Array<Token> const& tokens, Ref<Node>& tree );

    Opt<ParseError> parse_scope( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Scope& scope );
    Opt<ParseError> scope_return( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Node>& tree );
    Opt<ParseError> scope_break( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Node>& tree );
    Opt<ParseError> scope_continue( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Node>& tree );
    Opt<ParseError> scope_if( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Node>& tree );
    Opt<ParseError> scope_switch( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Node>& tree );
    Opt<ParseError> scope_loop( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Node>& tree );
    Opt<ParseError> scope_while( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Node>& tree );
    Opt<ParseError> scope_for( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Node>& tree );
};

Opt<ParseError> create_unary_node( Token const& token, Ref<UnaryNode>& node );
Opt<ParseError> create_operator_node( Token const& token, Ref<OperatorNode>& node );
Opt<ParseError> create_assign_node( Token const& token, Ref<AssignNode>& node );

Ref<Node> make_def_type_expr( StringRef const& type );
}
