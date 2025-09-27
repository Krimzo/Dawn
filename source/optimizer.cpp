#include "optimizer.h"


void dawn::Optimizer::optimize( Module& module )
{
    m_engine = Engine{};
    m_engine.load_mod( module );
    optimize_imports( module.imports );
    optimize_variables( module.variables );
    optimize_functions( module.functions );
    optimize_enums( module.enums );
    optimize_structs( module.structs );
}

void dawn::Optimizer::optimize_imports( Set<String>& imports )
{
    for ( auto& import : imports )
        optimize_import( const_cast<String&>( import ) );
}

void dawn::Optimizer::optimize_variables( Vector<Variable>& vars )
{
    for ( auto& var : vars )
        optimize_variable( var );
}

void dawn::Optimizer::optimize_functions( Vector<Function>& funcs )
{
    for ( auto& func : funcs )
        optimize_function( func );
}

void dawn::Optimizer::optimize_enums( Vector<Enum>& enums )
{
    for ( auto& enu : enums )
        optimize_enum( enu );
}

void dawn::Optimizer::optimize_structs( Vector<Struct>& structs )
{
    for ( auto& struc : structs )
        optimize_struct( struc );
}

void dawn::Optimizer::optimize_import( String& import )
{
    // None.
}

void dawn::Optimizer::optimize_variable( Variable& var )
{
    optimize_expr( var.expr.value() );
}

void dawn::Optimizer::optimize_function( Function& func )
{
    optimize_instr( func.body.instr );
}

void dawn::Optimizer::optimize_enum( Enum& enu )
{
    for ( auto& entry : enu.entries )
        optimize_expr( entry.expr.value() );
}

void dawn::Optimizer::optimize_struct( Struct& struc )
{
    for ( auto& field : struc.fields )
        optimize_expr( field.expr.value() );
    for ( auto& method : struc.methods )
        optimize_instr( method.body.instr );
}

void dawn::Optimizer::optimize_instr( Vector<Node>& body )
{
    for ( auto& instr : body )
        optimize_expr( instr );
}

void dawn::Optimizer::optimize_expr( Node& node )
{
    switch ( node.type() )
    {
    case NodeType::NONE: optimize_expr_none( std::get<None>( node ), node ); break;
    case NodeType::SCOPE: optimize_expr_scope( std::get<Scope>( node ), node ); break;
    case NodeType::VARIABLE: optimize_expr_var( std::get<VariableNode>( node ), node ); break;
    case NodeType::RETURN: optimize_expr_return( std::get<ReturnNode>( node ), node ); break;
    case NodeType::BREAK: optimize_expr_break( std::get<BreakNode>( node ), node ); break;
    case NodeType::CONTINUE: optimize_expr_continue( std::get<ContinueNode>( node ), node ); break;
    case NodeType::THROW: optimize_expr_throw( std::get<ThrowNode>( node ), node ); break;
    case NodeType::TRY: optimize_expr_try( std::get<TryNode>( node ), node ); break;
    case NodeType::IF: optimize_expr_if( std::get<IfNode>( node ), node ); break;
    case NodeType::SWITCH: optimize_expr_switch( std::get<SwitchNode>( node ), node ); break;
    case NodeType::LOOP: optimize_expr_loop( std::get<LoopNode>( node ), node ); break;
    case NodeType::WHILE: optimize_expr_while( std::get<WhileNode>( node ), node ); break;
    case NodeType::FOR: optimize_expr_for( std::get<ForNode>( node ), node ); break;
    case NodeType::VALUE: optimize_expr_value( std::get<ValueNode>( node ), node ); break;
    case NodeType::IDENTIFIER: optimize_expr_id( std::get<IdentifierNode>( node ), node ); break;
    case NodeType::CALL: optimize_expr_call( std::get<CallNode>( node ), node ); break;
    case NodeType::INDEX: optimize_expr_index( std::get<IndexNode>( node ), node ); break;
    case NodeType::ENUM: optimize_expr_enum( std::get<EnumNode>( node ), node ); break;
    case NodeType::STRUCT: optimize_expr_struct( std::get<StructNode>( node ), node ); break;
    case NodeType::ARRAY: optimize_expr_array( std::get<ArrayNode>( node ), node ); break;
    case NodeType::UNARY: optimize_expr_unary( std::get<UnaryNode>( node ), node ); break;
    case NodeType::OPERATOR: optimize_expr_op( std::get<OperatorNode>( node ), node ); break;
    case NodeType::ASSIGN: optimize_expr_as( std::get<AssignNode>( node ), node ); break;
    }
}

void dawn::Optimizer::optimize_expr_none( None& node, Node& out_node )
{
    // None.
}

void dawn::Optimizer::optimize_expr_scope( Scope& node, Node& out_node )
{
    optimize_instr( node.instr );
}

void dawn::Optimizer::optimize_expr_var( VariableNode& node, Node& out_node )
{
    optimize_expr( node.var.expr.value() );
}

void dawn::Optimizer::optimize_expr_return( ReturnNode& node, Node& out_node )
{
    optimize_expr( node.expr.value() );
}

void dawn::Optimizer::optimize_expr_break( BreakNode& node, Node& out_node )
{
    // None.
}

void dawn::Optimizer::optimize_expr_continue( ContinueNode& node, Node& out_node )
{
    // None.
}

void dawn::Optimizer::optimize_expr_throw( ThrowNode& node, Node& out_node )
{
    optimize_expr( node.expr.value() );
}

void dawn::Optimizer::optimize_expr_try( TryNode& node, Node& out_node )
{
    optimize_instr( node.try_scope.instr );
    optimize_instr( node.catch_scope.instr );
}

void dawn::Optimizer::optimize_expr_if( IfNode& node, Node& out_node )
{
    // 1. remove parts that are always false
    // 2. convert first part that is always true to else (by removing the ones after)
    for ( auto& part : node.parts )
    {
        optimize_expr( part.expr );
        optimize_instr( part.scope.instr );
    }
}

void dawn::Optimizer::optimize_expr_switch( SwitchNode& node, Node& out_node )
{
    optimize_expr( node.main_expr.value() );
    for ( auto& casee : node.cases )
    {
        for ( auto& expr : casee.exprs )
            optimize_expr( expr );
        optimize_instr( casee.scope.instr );
    }
    if ( node.def_scope )
        optimize_instr( node.def_scope->instr );
}

void dawn::Optimizer::optimize_expr_loop( LoopNode& node, Node& out_node )
{
    optimize_instr( node.scope.instr );
}

void dawn::Optimizer::optimize_expr_while( WhileNode& node, Node& out_node )
{
    optimize_expr( node.expr.value() );
    optimize_instr( node.scope.instr );
}

void dawn::Optimizer::optimize_expr_for( ForNode& node, Node& out_node )
{
    optimize_expr( node.expr.value() );
    optimize_instr( node.scope.instr );
}

void dawn::Optimizer::optimize_expr_value( ValueNode& node, Node& out_node )
{
    // None.
}

void dawn::Optimizer::optimize_expr_id( IdentifierNode& node, Node& out_node )
{
    // 1. find coresponding var and inline it if its const aka let
}

void dawn::Optimizer::optimize_expr_call( CallNode& node, Node& out_node )
{
    optimize_expr( node.left_expr.value() );
    for ( auto& arg : node.args )
        optimize_expr( arg );
}

void dawn::Optimizer::optimize_expr_index( IndexNode& node, Node& out_node )
{
    optimize_expr( node.left_expr.value() );
    optimize_expr( node.expr.value() );
}

void dawn::Optimizer::optimize_expr_enum( EnumNode& node, Node& out_node )
{
    // None.
}

void dawn::Optimizer::optimize_expr_struct( StructNode& node, Node& out_node )
{
    if ( std::holds_alternative<StructNode::NamedInit>( node.init ) )
    {
        for ( auto& [_, arg] : std::get<StructNode::NamedInit>( node.init ).args )
            optimize_expr( arg );
    }
    else
    {
        for ( auto& arg : std::get<StructNode::ListInit>( node.init ).args )
            optimize_expr( arg );
    }
}

void dawn::Optimizer::optimize_expr_array( ArrayNode& node, Node& out_node )
{
    if ( std::holds_alternative<ArrayNode::ListInit>( node.init ) )
    {
        for ( auto& element : std::get<ArrayNode::ListInit>( node.init ).elements )
            optimize_expr( element );
    }
    else
    {
        optimize_expr( std::get<ArrayNode::SizedInit>( node.init ).size_expr.value() );
    }
}

void dawn::Optimizer::optimize_expr_unary( UnaryNode& node, Node& out_node )
{
    auto& right_node = node.right.value();
    optimize_expr( right_node );
    if ( right_node.type() != NodeType::VALUE )
        return;

    auto& val_node = std::get<ValueNode>( right_node );
    auto& value = val_node.value;
    switch ( node.type )
    {
    case UnaryType::PLUS: value = value.un_plus( m_engine, node.location ); break;
    case UnaryType::MINUS: value = value.un_minus( m_engine, node.location ); break;
    case UnaryType::NOT: value = value.un_not( m_engine, node.location ); break;
    }
    out_node.emplace<ValueNode>( ValueNode{ val_node } ); // Must copy into another ValueNode{} since out_node is the parent of node.
}

void dawn::Optimizer::optimize_expr_op( OperatorNode& node, Node& out_node )
{
    auto& left_node = node.sides[0];
    optimize_expr( left_node );
    if ( left_node.type() != NodeType::VALUE )
        return;

    auto& right_node = node.sides[1];
    optimize_expr( right_node );
    if ( right_node.type() != NodeType::VALUE )
        return;

    auto& left_val_node = std::get<ValueNode>( left_node );
    auto& right_val_node = std::get<ValueNode>( right_node );
    auto& left_value = left_val_node.value;
    auto& right_value = right_val_node.value;
    switch ( node.type )
    {
    case OperatorType::ACCESS: return; // 1. could optimize access of let vars
    case OperatorType::POW: left_value = left_value.op_pow( m_engine, node.location, right_value ); break;
    case OperatorType::MOD: left_value = left_value.op_mod( m_engine, node.location, right_value ); break;
    case OperatorType::MUL: left_value = left_value.op_mul( m_engine, node.location, right_value ); break;
    case OperatorType::DIV: left_value = left_value.op_div( m_engine, node.location, right_value ); break;
    case OperatorType::ADD: left_value = left_value.op_add( m_engine, node.location, right_value ); break;
    case OperatorType::SUB: left_value = left_value.op_sub( m_engine, node.location, right_value ); break;
    case OperatorType::COMPARE: left_value = left_value.op_cmpr( m_engine, node.location, right_value ); break;
    case OperatorType::LESS: left_value = left_value.op_less( m_engine, node.location, right_value ); break;
    case OperatorType::GREAT: left_value = left_value.op_great( m_engine, node.location, right_value ); break;
    case OperatorType::LESS_EQ: left_value = left_value.op_lesseq( m_engine, node.location, right_value ); break;
    case OperatorType::GREAT_EQ: left_value = left_value.op_greateq( m_engine, node.location, right_value ); break;
    case OperatorType::EQ: left_value = left_value.op_eq( m_engine, node.location, right_value ); break;
    case OperatorType::NOT_EQ: left_value = left_value.op_neq( m_engine, node.location, right_value ); break;
    case OperatorType::AND: left_value = left_value.op_and( m_engine, node.location, right_value ); break;
    case OperatorType::OR: left_value = left_value.op_or( m_engine, node.location, right_value ); break;
    case OperatorType::RANGE: left_value = left_value.op_range( m_engine, node.location, right_value ); break;
    }
    out_node.emplace<ValueNode>( ValueNode{ left_val_node } ); // Must copy into another ValueNode{} since out_node is the parent of node.
}

void dawn::Optimizer::optimize_expr_as( AssignNode& node, Node& out_node )
{
    optimize_expr( node.sides[1] );
}
