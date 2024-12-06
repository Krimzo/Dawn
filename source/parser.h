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
            stream << L"Parse error at token " << *token << L": ";
        }
        else
        {
            stream << L"Parse error: ";
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
    String name;
    Map<String, Variable> variables;
    Map<String, Operator> operators;
    Map<String, Function> functions;
    Map<String, EnumType> enums;
    Map<String, LayerType> layers;
    Map<String, StructType> structs;

    Bool contains_id( StringRef const& id ) const;
};

struct Parser
{
    Opt<ParseError> parse( Array<Token>& tokens, Module& module );

private:
    void prepare_tokens( Array<Token>& tokens );

    Opt<ParseError> parse_module_module( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module );
    Opt<ParseError> parse_module_struct( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module );
    Opt<ParseError> parse_module_layer( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module );
    Opt<ParseError> parse_module_enum( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module );
    Opt<ParseError> parse_module_function( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module );
    Opt<ParseError> parse_module_operator( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module );
    Opt<ParseError> parse_module_variable( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module );

    Opt<ParseError> parse_struct( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, StructType& struct_type );
    Opt<ParseError> parse_layer( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, LayerType& layer_type );
    Opt<ParseError> parse_enum( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, EnumType& enum_type );
    Opt<ParseError> parse_function( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Function& function );
    Opt<ParseError> parse_operator( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Operator& operato );
    Opt<ParseError> parse_variable( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Variable& variable );

    Opt<ParseError> parse_type( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Type>& type );
    Opt<ParseError> type_basic( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Type>& type );
    Opt<ParseError> type_reference( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<RefType>& type );

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
    Opt<ParseError> expression_yield( Array<Token> const& tokens, Ref<Node>& tree );

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
}
