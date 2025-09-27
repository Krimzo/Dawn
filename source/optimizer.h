#pragma once

#include "engine.h"


namespace dawn
{
struct Optimizer
{
    void optimize( Module& module );

private:
    Engine m_engine;

    void optimize_imports( Set<String>& imports );
    void optimize_variables( Vector<Variable>& vars );
    void optimize_functions( Vector<Function>& funcs );
    void optimize_enums( Vector<Enum>& enums );
    void optimize_structs( Vector<Struct>& structs );

    void optimize_import( String& import );
    void optimize_variable( Variable& var );
    void optimize_function( Function& func );
    void optimize_enum( Enum& enu );
    void optimize_struct( Struct& struc );

    void optimize_instr( Vector<Node>& body );
    void optimize_expr( Node& node );

    void optimize_expr_none( None& node, Node& out_node );
    void optimize_expr_scope( Scope& node, Node& out_node );
    void optimize_expr_var( VariableNode& node, Node& out_node );
    void optimize_expr_return( ReturnNode& node, Node& out_node );
    void optimize_expr_break( BreakNode& node, Node& out_node );
    void optimize_expr_continue( ContinueNode& node, Node& out_node );
    void optimize_expr_throw( ThrowNode& node, Node& out_node );
    void optimize_expr_try( TryNode& node, Node& out_node );
    void optimize_expr_if( IfNode& node, Node& out_node );
    void optimize_expr_switch( SwitchNode& node, Node& out_node );
    void optimize_expr_loop( LoopNode& node, Node& out_node );
    void optimize_expr_while( WhileNode& node, Node& out_node );
    void optimize_expr_for( ForNode& node, Node& out_node );
    void optimize_expr_value( ValueNode& node, Node& out_node );
    void optimize_expr_id( IdentifierNode& node, Node& out_node );
    void optimize_expr_call( CallNode& node, Node& out_node );
    void optimize_expr_index( IndexNode& node, Node& out_node );
    void optimize_expr_enum( EnumNode& node, Node& out_node );
    void optimize_expr_struct( StructNode& node, Node& out_node );
    void optimize_expr_array( ArrayNode& node, Node& out_node );
    void optimize_expr_unary( UnaryNode& node, Node& out_node );
    void optimize_expr_op( OperatorNode& node, Node& out_node );
    void optimize_expr_as( AssignNode& node, Node& out_node );
};
}
