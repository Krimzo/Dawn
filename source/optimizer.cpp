#include "optimizer.h"


void dawn::Optimizer::optimize( Module& module )
{
    reset();
    m_engine.load_mod( module );
    optimize_imports( module.imports );
    optimize_variables( module.variables );
    optimize_operators( module.operators );
    optimize_functions( module.functions );
    optimize_enums( module.enums );
    optimize_structs( module.structs );
}

void dawn::Optimizer::reset()
{
    m_engine = {};
    m_inline.clear();
}

void dawn::Optimizer::optimize_imports( StringSet& imports )
{
    for ( auto& import : imports )
        optimize_import( const_cast<String&>( import ) );
}

void dawn::Optimizer::optimize_variables( Vector<Variable>& vars )
{
    for ( auto& var : vars )
        optimize_variable( var );
}

void dawn::Optimizer::optimize_operators( Vector<Operator>& ops )
{
    for ( auto& op : ops )
        optimize_operator( op );
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
    optimize_expr( *var.expr );
}

void dawn::Optimizer::optimize_operator( Operator& op )
{
    const InlineDropper inline_dropper{ m_inline };
    for ( auto& arg : op.args )
        m_inline.emplace_back( arg.id );
    optimize_instr( op.body.instr );
}

void dawn::Optimizer::optimize_function( Function& func )
{
    const InlineDropper inline_dropper{ m_inline };
    for ( auto& arg : func.args )
        m_inline.emplace_back( arg.id );
    optimize_instr( func.body.instr );
}

void dawn::Optimizer::optimize_enum( Enum& enu )
{
    for ( auto& entry : enu.entries )
    {
        if ( auto* expr_refptr = std::get_if<NodeRef>( &entry.expr ) )
            optimize_expr( **expr_refptr );
    }
}

void dawn::Optimizer::optimize_struct( Struct& struc )
{
    for ( auto& method : struc.methods )
    {
        const InlineDropper inline_dropper{ m_inline };
        for ( auto& arg : method.args )
            m_inline.emplace_back( arg.id );
        optimize_instr( method.body.instr );
    }
}

void dawn::Optimizer::optimize_instr( Vector<Node>& scope )
{
    const InlineDropper inline_dropper{ m_inline };
    for ( Int i = 0; i < (Int) scope.size(); i++ )
    {
        auto& instr = scope[i];
        optimize_expr( instr );
        if ( instr.type() == NodeType::VARIABLE )
            inline_var( std::get<VariableNode>( instr ).var, scope, i );
    }
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
    case NodeType::VALUE: optimize_expr_value( std::get<Value>( node ), node ); break;
    case NodeType::IDENTIFIER: optimize_expr_id( std::get<IdentifierNode>( node ), node ); break;
    case NodeType::CALL: optimize_expr_call( std::get<CallNode>( node ), node ); break;
    case NodeType::INDEX: optimize_expr_index( std::get<IndexNode>( node ), node ); break;
    case NodeType::LAMBDA: optimize_expr_lambda( std::get<LambdaNode>( node ), node ); break;
    case NodeType::ENUM: optimize_expr_enum( std::get<EnumNode>( node ), node ); break;
    case NodeType::STRUCT: optimize_expr_struct( std::get<StructNode>( node ), node ); break;
    case NodeType::ARRAY: optimize_expr_array( std::get<ArrayNode>( node ), node ); break;
    case NodeType::ACCESS: optimize_expr_ac( std::get<AccessNode>( node ), node ); break;
    case NodeType::OPERATOR: optimize_expr_op( std::get<OperatorNode>( node ), node ); break;
    case NodeType::ASSIGN: optimize_expr_as( std::get<AssignNode>( node ), node ); break;
    }
}

void dawn::Optimizer::inline_var( Variable& var, Vector<Node>& scope, Int& i )
{
    if ( var.type.kind != VarKind::CONSTANT && var.type.kind != VarKind::REFERENCE )
    {
        m_inline.emplace_back( var.id );
        return;
    }

    auto& expr = *var.expr;
    if ( expr.type() != NodeType::VALUE )
    {
        m_inline.emplace_back( var.id );
        return;
    }

    auto& value = std::get<Value>( expr );
    if ( !value.is_const() )
    {
        m_inline.emplace_back( var.id );
        return;
    }

    if ( value.type_id() != var.type.type_id )
        ENGINE_PANIC( expr.location(), "optimizer can not inline variable of type [", IDSystem::get( var.type.type_id ), "] because expr is of type [", IDSystem::get( value.type_id() ), "]" );

    m_inline.emplace_back( var.id, value, true );
    scope.erase( scope.begin() + i );
    --i;
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
    optimize_expr( *node.var.expr );
}

void dawn::Optimizer::optimize_expr_return( ReturnNode& node, Node& out_node )
{
    optimize_expr( *node.expr );
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
    optimize_expr( *node.expr );
}

void dawn::Optimizer::optimize_expr_try( TryNode& node, Node& out_node )
{
    if ( !node.try_scope.instr.empty() )
    {
        optimize_instr( node.try_scope.instr );
        optimize_instr( node.catch_scope.instr );
    }
    else
        out_node.emplace<Scope>();
}

void dawn::Optimizer::optimize_expr_if( IfNode& node, Node& out_node )
{
    for ( Int i = 0; i < (Int) node.parts.size(); i++ )
    {
        auto& part = node.parts[i];
        optimize_expr( part.expr );
        if ( part.expr.type() != NodeType::VALUE )
        {
            optimize_instr( part.scope.instr );
            continue;
        }

        if ( std::get<Value>( part.expr ).as_bool() )
        {
            optimize_instr( part.scope.instr ); // Optimize before resizing (invalid reference otherwise).
            node.parts.resize( i + 1 );
            break;
        }
        else
        {
            node.parts.erase( node.parts.begin() + i );
            --i;
        }
    }
    if ( node.parts.size() == 1 && node.parts.front().expr.type() == NodeType::VALUE )
    {
        auto& part = node.parts.front();
        out_node.emplace<Scope>( Scope{ std::move( part.scope ) } );
    }
}

void dawn::Optimizer::optimize_expr_switch( SwitchNode& node, Node& out_node )
{
    auto& main_expr = *node.main_expr;
    optimize_expr( main_expr );
    const Bool main_is_val = ( main_expr.type() == NodeType::VALUE );
    for ( Int i = 0; i < (Int) node.cases.size(); i++ )
    {
        auto& casee = node.cases[i];
        Int is_value_counter = 0;
        for ( auto& expr : casee.exprs )
        {
            optimize_expr( expr );
            if ( !main_is_val || expr.type() != NodeType::VALUE )
                continue;
            ++is_value_counter;

            if ( m_engine.handle_oper( expr.location(), std::get<Value>( main_expr ), OperatorType::EQ, std::get<Value>( expr ) ).as_bool() )
            {
                optimize_instr( casee.scope.instr );
                out_node.emplace<Scope>( Scope{ std::move( casee.scope ) } );
                return;
            }
        }
        if ( main_is_val && is_value_counter == (Int) casee.exprs.size() )
        {
            node.cases.erase( node.cases.begin() + i );
            --i;
        }
        else
            optimize_instr( casee.scope.instr );
    }
    if ( node.def_scope )
    {
        optimize_instr( node.def_scope->instr );
        if ( node.cases.empty() )
            out_node.emplace<Scope>( Scope{ std::move( *node.def_scope ) } );
    }
    else if ( node.cases.size() == 1 )
        out_node.emplace<Scope>( Scope{ std::move( node.cases.front().scope ) } );
}

void dawn::Optimizer::optimize_expr_loop( LoopNode& node, Node& out_node )
{
    if ( !node.scope.instr.empty() )
        optimize_instr( node.scope.instr );
    else
        out_node.emplace<Scope>();
}

void dawn::Optimizer::optimize_expr_while( WhileNode& node, Node& out_node )
{
    auto& expr_node = *node.expr;
    optimize_expr( expr_node );
    optimize_instr( node.scope.instr );
    if ( expr_node.type() == NodeType::VALUE )
    {
        if ( std::get<Value>( expr_node ).as_bool() )
            out_node.emplace<LoopNode>( node.location ).scope = Scope{ std::move( node.scope ) };
        else
            out_node.emplace<Scope>();
    }
}

void dawn::Optimizer::optimize_expr_for( ForNode& node, Node& out_node )
{
    auto& expr_node = *node.expr;
    optimize_expr( expr_node );
    if ( expr_node.type() == NodeType::VALUE )
    {
        auto const& expr_value = std::get<Value>( expr_node );
        switch ( expr_value.type() )
        {
        case ValueType::STRING:
            if ( expr_value.as_string().empty() )
            {
                out_node.emplace<Scope>();
                return;
            }
            break;

        case ValueType::RANGE:
            if ( expr_value.as_range().empty() )
            {
                out_node.emplace<Scope>();
                return;
            }
            break;

        case ValueType::ARRAY:
            if ( expr_value.as_array().data.empty() )
            {
                out_node.emplace<Scope>();
                return;
            }
            break;
        }
    }
    optimize_instr( node.scope.instr );
}

void dawn::Optimizer::optimize_expr_value( Value& value, Node& out_node )
{
    // None.
}

void dawn::Optimizer::optimize_expr_id( IdentifierNode& node, Node& out_node )
{
    for ( auto it = m_inline.rbegin(); it != m_inline.rend(); ++it )
    {
        auto const& [id, value, can_inline] = *it;
        if ( id != node.id )
            continue;
        if ( !can_inline )
            return;
        out_node.emplace<Value>( it->value );
        break;
    }
}

void dawn::Optimizer::optimize_expr_call( CallNode& node, Node& out_node )
{
    Bool is_ctime = true;
    auto& left_expr = *node.left_expr;
    optimize_expr( left_expr );
    if ( left_expr.type() != NodeType::IDENTIFIER || !m_engine.m_ctime_funcs.contains( std::get<IdentifierNode>( left_expr ).id ) )
        is_ctime = false;
    for ( auto& arg : node.args )
    {
        optimize_expr( arg );
        if ( arg.type() != NodeType::VALUE )
            is_ctime = false;
    }
    if ( is_ctime )
        out_node.emplace<Value>( m_engine.handle_call_node( node ) );
}

void dawn::Optimizer::optimize_expr_index( IndexNode& node, Node& out_node )
{
    auto& left_node = *node.left_expr;
    optimize_expr( left_node );
    auto& expr_node = *node.expr;
    optimize_expr( expr_node );
    if ( left_node.type() == NodeType::VALUE && expr_node.type() == NodeType::VALUE )
        out_node.emplace<Value>( m_engine.handle_index_node( node ) );
}

void dawn::Optimizer::optimize_expr_lambda( LambdaNode& node, Node& out_node )
{
    auto& func = *node.func_value.as_function().dfunction();
    const InlineDropper inline_dropper{ m_inline };
    for ( auto& arg : func.args )
        m_inline.emplace_back( arg.id );
    optimize_instr( func.body.instr );
}

void dawn::Optimizer::optimize_expr_enum( EnumNode& node, Node& out_node )
{
    // None.
}

void dawn::Optimizer::optimize_expr_struct( StructNode& node, Node& out_node )
{
    if ( std::holds_alternative<StructNode::NamedInit>( node.init ) )
    {
        Bool is_ctime = true;
        for ( auto& [_, arg] : std::get<StructNode::NamedInit>( node.init ).args )
        {
            optimize_expr( arg );
            if ( arg.type() != NodeType::VALUE )
                is_ctime = false;
        }
        if ( is_ctime )
            out_node.emplace<Value>( m_engine.handle_struct_node( node ) );
    }
    else
    {
        Bool is_ctime = true;
        for ( auto& arg : std::get<StructNode::ListInit>( node.init ).args )
        {
            optimize_expr( arg );
            if ( arg.type() != NodeType::VALUE )
                is_ctime = false;
        }
        if ( is_ctime )
            out_node.emplace<Value>( m_engine.handle_struct_node( node ) );
    }
}

void dawn::Optimizer::optimize_expr_array( ArrayNode& node, Node& out_node )
{
    if ( std::holds_alternative<ArrayNode::ListInit>( node.init ) )
    {
        Bool is_ctime = true;
        for ( auto& element : std::get<ArrayNode::ListInit>( node.init ).elements )
        {
            optimize_expr( element );
            if ( element.type() != NodeType::VALUE )
                is_ctime = false;
        }
        if ( is_ctime )
            out_node.emplace<Value>( m_engine.handle_array_node( node ) );
    }
    else
    {
        auto& init = std::get<ArrayNode::SizedInit>( node.init );
        auto& size_expr = *init.size_expr;
        optimize_expr( size_expr );
        if ( size_expr.type() == NodeType::VALUE )
            out_node.emplace<Value>( m_engine.handle_array_node( node ) );
    }
}

void dawn::Optimizer::optimize_expr_ac( AccessNode& node, Node& out_node )
{
    optimize_expr( *node.left_expr );
}

void dawn::Optimizer::optimize_expr_op( OperatorNode& node, Node& out_node )
{
    auto& left_node = node.sides[0];
    auto& right_node = node.sides[1];
    optimize_expr( left_node );
    optimize_expr( right_node );

    if ( left_node.type() != NodeType::VALUE || right_node.type() != NodeType::VALUE )
        return;

    Value left_value = std::get<Value>( left_node );
    auto& right_value = std::get<Value>( right_node );
    if ( !m_engine.m_ctime_ops[(Int) node.type].contains( combine_ids( left_value.type_id(), right_value.type_id() ) ) )
        return;

    left_value = m_engine.handle_oper( node.location, left_value, node.type, right_value );
    out_node.emplace<Value>( left_value );
}

void dawn::Optimizer::optimize_expr_as( AssignNode& node, Node& out_node )
{
    for ( auto& side : node.sides )
        optimize_expr( side );
}
