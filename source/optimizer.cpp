#include "optimizer.h"


void dawn::Optimizer::optimize( Module& module )
{
    reset();
    m_engine.load_mod( module );
    optimize_imports( module.imports );
    optimize_variables( module.variables );
    optimize_functions( module.functions );
    optimize_enums( module.enums );
    optimize_structs( module.structs );
}

void dawn::Optimizer::reset()
{
    m_engine = {};
    m_ctime_funcs = m_engine.ctime_funcs();
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
    const InlineDropper inline_dropper{ m_inline };
    for ( auto& arg : func.args )
        m_inline.emplace_back( arg.id );
    optimize_instr( func.body.instr );
}

void dawn::Optimizer::optimize_enum( Enum& enu )
{
    for ( auto& entry : enu.entries )
    {
        if ( std::holds_alternative<NodeRef>( entry.expr ) )
            optimize_expr( std::get<NodeRef>( entry.expr ).value() );
    }
}

void dawn::Optimizer::optimize_struct( Struct& struc )
{
    for ( auto& field : struc.fields )
        optimize_expr( field.expr.value() );
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
        if ( instr.type() != NodeType::VARIABLE )
            continue;

        auto& var = std::get<VariableNode>( instr ).var;
        if ( var.kind != VariableKind::LET && var.kind != VariableKind::REF )
        {
            m_inline.emplace_back( var.id );
            continue;
        }

        auto& expr = var.expr.value();
        if ( expr.type() != NodeType::VALUE )
        {
            m_inline.emplace_back( var.id );
            continue;
        }

        auto& value = std::get<ValueNode>( expr ).value;
        if ( !value.is_const() )
        {
            m_inline.emplace_back( var.id );
            continue;
        }

        m_inline.emplace_back( var.id, value, true );
        scope.erase( scope.begin() + i );
        --i;
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
    case NodeType::VALUE: optimize_expr_value( std::get<ValueNode>( node ), node ); break;
    case NodeType::IDENTIFIER: optimize_expr_id( std::get<IdentifierNode>( node ), node ); break;
    case NodeType::CALL: optimize_expr_call( std::get<CallNode>( node ), node ); break;
    case NodeType::INDEX: optimize_expr_index( std::get<IndexNode>( node ), node ); break;
    case NodeType::LAMBDA: optimize_expr_lambda( std::get<LambdaNode>( node ), node ); break;
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
    for ( Int i = 0; i < (Int) node.parts.size(); i++ )
    {
        auto& part = node.parts[i];
        optimize_expr( part.expr );
        if ( part.expr.type() != NodeType::VALUE )
        {
            optimize_instr( part.scope.instr );
            continue;
        }

        auto const& value_node = std::get<ValueNode>( part.expr );
        if ( value_node.value.to_bool( value_node.location ) )
        {
            optimize_instr( part.scope.instr ); // Optimize before resizing (invalid reference otherwise).
            node.parts.resize( i + 1 );
            node.parts.back() // Store direct true so that to_bool is not computed again.
                .expr.emplace<ValueNode>( value_node.location )
                .value = Value{ Bool{ true } };
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
    auto& main_expr = node.main_expr.value();
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

            auto const& expr_val = std::get<ValueNode>( expr ).value;
            if ( std::get<ValueNode>( main_expr ).value.op_eq( expr.location(), expr_val ).as_bool() )
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
    optimize_instr( node.scope.instr );
}

void dawn::Optimizer::optimize_expr_while( WhileNode& node, Node& out_node )
{
    auto& expr_node = node.expr.value();
    optimize_expr( expr_node );
    optimize_instr( node.scope.instr );
    if ( expr_node.type() == NodeType::VALUE )
    {
        if ( std::get<ValueNode>( expr_node ).value.to_bool( expr_node.location() ) )
            out_node.emplace<LoopNode>( node.location ).scope = Scope{ std::move( node.scope ) };
        else
            out_node.emplace<Scope>();
    }
}

void dawn::Optimizer::optimize_expr_for( ForNode& node, Node& out_node )
{
    auto& expr_node = node.expr.value();
    optimize_expr( expr_node );
    if ( expr_node.type() == NodeType::VALUE )
    {
        auto const& expr_value = std::get<ValueNode>( expr_node ).value;
        switch ( expr_value.type() )
        {
        case ValueType::RANGE:
            if ( expr_value.as_range().empty() )
            {
                out_node.emplace<Scope>();
                return;
            }
            break;

        case ValueType::STRING:
            if ( expr_value.as_string().empty() )
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

void dawn::Optimizer::optimize_expr_value( ValueNode& node, Node& out_node )
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
        ValueNode valnod{ node.location };
        valnod.value = it->value;
        out_node.emplace<ValueNode>( std::move( valnod ) );
        break;
    }
}

void dawn::Optimizer::optimize_expr_call( CallNode& node, Node& out_node )
{
    Bool is_ctime = true;
    auto& left_expr = node.left_expr.value();
    optimize_expr( left_expr );
    if ( left_expr.type() != NodeType::IDENTIFIER || !m_ctime_funcs.contains( std::get<IdentifierNode>( left_expr ).id ) )
        is_ctime = false;
    for ( auto& arg : node.args )
    {
        optimize_expr( arg );
        if ( arg.type() != NodeType::VALUE )
            is_ctime = false;
    }
    if ( is_ctime )
        out_node.emplace<ValueNode>( node.location ).value = m_engine.handle_call_node( node );
}

void dawn::Optimizer::optimize_expr_index( IndexNode& node, Node& out_node )
{
    optimize_expr( node.left_expr.value() );
    optimize_expr( node.expr.value() );
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
        Bool is_ctime = true;
        for ( auto& element : std::get<ArrayNode::ListInit>( node.init ).elements )
        {
            optimize_expr( element );
            if ( element.type() != NodeType::VALUE )
                is_ctime = false;
        }
        if ( is_ctime )
            out_node.emplace<ValueNode>( node.location ).value = m_engine.handle_array_node( node );
    }
    else
    {
        auto& init = std::get<ArrayNode::SizedInit>( node.init );
        auto& size_expr = init.size_expr.value();
        optimize_expr( size_expr );
        if ( size_expr.type() == NodeType::VALUE && !is_custom_type( IDSystem::get( init.type_id ) ) )
            out_node.emplace<ValueNode>( node.location ).value = m_engine.handle_array_node( node );
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
    case UnaryType::PLUS: value = value.un_plus( node.location ); break;
    case UnaryType::MINUS: value = value.un_minus( node.location ); break;
    case UnaryType::NOT: value = value.un_not( node.location ); break;
    }
    out_node.emplace<ValueNode>( ValueNode{ val_node } ); // Must copy into another ValueNode{} since out_node is the parent of node.
}

void dawn::Optimizer::optimize_expr_op( OperatorNode& node, Node& out_node )
{
    auto& left_node = node.sides[0];
    auto& right_node = node.sides[1];
    optimize_expr( left_node );
    optimize_expr( right_node );
    if ( left_node.type() != NodeType::VALUE || right_node.type() != NodeType::VALUE )
        return;

    auto& left_val_node = std::get<ValueNode>( left_node );
    auto& right_val_node = std::get<ValueNode>( right_node );
    auto& left_value = left_val_node.value;
    auto& right_value = right_val_node.value;
    switch ( node.type )
    {
    case OperatorType::ACCESS: return; // 1. could optimize access of let vars
    case OperatorType::POW: left_value = left_value.op_pow( node.location, right_value ); break;
    case OperatorType::MOD: left_value = left_value.op_mod( node.location, right_value ); break;
    case OperatorType::MUL: left_value = left_value.op_mul( node.location, right_value ); break;
    case OperatorType::DIV: left_value = left_value.op_div( node.location, right_value ); break;
    case OperatorType::ADD: left_value = left_value.op_add( node.location, right_value ); break;
    case OperatorType::SUB: left_value = left_value.op_sub( node.location, right_value ); break;
    case OperatorType::COMPARE: left_value = left_value.op_cmpr( node.location, right_value ); break;
    case OperatorType::LESS: left_value = left_value.op_less( node.location, right_value ); break;
    case OperatorType::GREAT: left_value = left_value.op_great( node.location, right_value ); break;
    case OperatorType::LESS_EQ: left_value = left_value.op_lesseq( node.location, right_value ); break;
    case OperatorType::GREAT_EQ: left_value = left_value.op_greateq( node.location, right_value ); break;
    case OperatorType::EQ: left_value = left_value.op_eq( node.location, right_value ); break;
    case OperatorType::NOT_EQ: left_value = left_value.op_neq( node.location, right_value ); break;
    case OperatorType::AND: left_value = left_value.op_and( node.location, right_value ); break;
    case OperatorType::OR: left_value = left_value.op_or( node.location, right_value ); break;
    case OperatorType::RANGE: left_value = left_value.op_range( node.location, right_value ); break;
    }
    out_node.emplace<ValueNode>( ValueNode{ left_val_node } ); // Must copy into another ValueNode{} since out_node is the parent of node.
}

void dawn::Optimizer::optimize_expr_as( AssignNode& node, Node& out_node )
{
    for ( auto& side : node.sides )
        optimize_expr( side );
}
