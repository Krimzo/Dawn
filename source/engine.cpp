#include "engine.h"


dawn::Engine::Engine( Global& global )
    : global{ global }
    , stack{ global.values }
{
}

void dawn::Engine::handle_var_node( VariableNode const& node )
{
    add_var( node.location, node.var.type, node.var.id, handle_expr( *node.var.expr ) );
}

dawn::Value const& dawn::Engine::handle_id_node( IdentifierNode const& node )
{
    if ( auto* ptr = stack.get( node.id ) )
        return *ptr;
    ENGINE_PANIC( node.location, "object [", IDSystem::get( node.id ), "] does not exist" );
}

dawn::Value dawn::Engine::handle_call_node( CallNode const& node )
{
    const Value left = handle_expr( *node.left_expr );
    if ( left.type() != ValueType::FUNCTION )
        ENGINE_PANIC( node.location, "can not call [", left.type(), "]" );

    auto const& func = left.as_function();
    const Int arg_count = func.is_method() ? ( 1 + node.args.size() ) : node.args.size();

    Value* const args_ptr = SALLOC( Value, arg_count );
    const SAllocManager<Value> alloc_manager{ args_ptr, arg_count };

    if ( func.is_method() )
    {
        args_ptr[0] = *func.as_method().self;
        for ( Int i = 0; i < (Int) node.args.size(); i++ )
            args_ptr[1 + i] = handle_expr( node.args[i] );
    }
    else
    {
        for ( Int i = 0; i < (Int) node.args.size(); i++ )
            args_ptr[i] = handle_expr( node.args[i] );
    }

    return handle_func( node.location, func, args_ptr, arg_count );
}

dawn::Value dawn::Engine::handle_index_node( IndexNode const& node )
{
    Value left = handle_expr( *node.left_expr );
    Int index = handle_expr( *node.expr ).as_int();

    if ( left.type() == ValueType::STRING )
    {
        auto& value = left.as_string();
        if ( index < 0 || index >= (Int) value.size() )
            ENGINE_PANIC( node.location, "string access [", index, "] out of bounds" );
        return Value{ value[index], node.location };
    }
    else if ( left.type() == ValueType::ARRAY )
    {
        auto& value = left.as_array();
        if ( index < 0 || index >= (Int) value.data.size() )
            ENGINE_PANIC( node.location, "array access [", index, "] out of bounds" );
        return value.data[index];
    }
    else
        ENGINE_PANIC( node.location, "can not index type [", left.type(), "]" );
}

void dawn::Engine::handle_return_node( ReturnNode const& node, Opt<Value>& retval )
{
    retval = handle_expr( *node.expr );
}

void dawn::Engine::handle_break_node( BreakNode const& node, Bool* didbrk )
{
    if ( !didbrk )
        ENGINE_PANIC( node.location, "break statement outside of loop" );
    *didbrk = true;
}

void dawn::Engine::handle_continue_node( ContinueNode const& node, Bool* didcon )
{
    if ( !didcon )
        ENGINE_PANIC( node.location, "continue statement outside of loop" );
    *didcon = true;
}

void dawn::Engine::handle_throw_node( ThrowNode const& node )
{
    throw handle_expr( *node.expr );
}

void dawn::Engine::handle_try_node( TryNode const& node, Opt<Value>& retval, Bool* didbrk, Bool* didcon )
{
    try
    {
        auto pop_handler = stack.push();
        handle_scope( node.try_scope, retval, didbrk, didcon );
    }
    catch ( Value const& value )
    {
        auto pop_handler = stack.push();
        stack.set( node.catch_id, value );
        handle_scope( node.catch_scope, retval, didbrk, didcon );
    }
}

void dawn::Engine::handle_if_node( IfNode const& node, Opt<Value>& retval, Bool* didbrk, Bool* didcon )
{
    for ( auto& part : node.parts )
    {
        if ( !handle_expr( part.expr ).as_bool() )
            continue;

        auto pop_handler = stack.push();
        handle_scope( part.scope, retval, didbrk, didcon );
        break;
    }
}

void dawn::Engine::handle_switch_node( SwitchNode const& node, Opt<Value>& retval, Bool* didbrk, Bool* didcon )
{
    const Value check_value = handle_expr( *node.main_expr );

    for ( auto& case_part : node.cases )
    {
        for ( auto& expr : case_part.exprs )
        {
            if ( !handle_oper( expr.location(), check_value, OperatorType::EQ, handle_expr( expr ) ).as_bool() )
                continue;

            auto pop_handler = stack.push();
            handle_scope( case_part.scope, retval, didbrk, didcon );
            return;
        }
    }

    if ( node.def_scope )
    {
        auto pop_handler = stack.push();
        handle_scope( *node.def_scope, retval, didbrk, didcon );
    }
}

void dawn::Engine::handle_loop_node( LoopNode const& node, Opt<Value>& retval )
{
    Bool didbrk = false, didcon = false;
    while ( true )
    {
        if ( retval || didbrk )
            break;
        didcon = false;

        auto pop_handler = stack.push();
        handle_scope( node.scope, retval, &didbrk, &didcon );
    }
}

void dawn::Engine::handle_while_node( WhileNode const& node, Opt<Value>& retval )
{
    Bool didbrk = false, didcon = false;
    while ( true )
    {
        if ( retval || didbrk )
            break;
        didcon = false;

        if ( !handle_expr( *node.expr ).as_bool() )
            break;

        auto pop_handler = stack.push();
        handle_scope( node.scope, retval, &didbrk, &didcon );
    }
}

void dawn::Engine::handle_for_node( ForNode const& node, Opt<Value>& retval )
{
    Value loop_value = handle_expr( *node.expr );
    const auto value_type = loop_value.type();
    if ( value_type == ValueType::STRING )
    {
        auto& value_str = loop_value.as_string();

        Bool didbrk = false, didcon = false;
        for ( Char c : value_str )
        {
            if ( retval || didbrk )
                break;
            didcon = false;

            auto pop_handler = stack.push();
            stack.set( node.var_id, Value{ c, node.location } );
            handle_scope( node.scope, retval, &didbrk, &didcon );
        }
    }
    else if ( value_type == ValueType::RANGE )
    {
        auto& value_rng = loop_value.as_range();

        Bool didbrk = false, didcon = false;
        for ( Int i = value_rng.start_incl; i < value_rng.end_excl; ++i )
        {
            if ( retval || didbrk )
                break;
            didcon = false;

            auto pop_handler = stack.push();
            stack.set( node.var_id, Value{ i, node.location } );
            handle_scope( node.scope, retval, &didbrk, &didcon );
        }
    }
    else if ( value_type == ValueType::ARRAY )
    {
        auto& value_arr = loop_value.as_array();

        Bool didbrk = false, didcon = false;
        for ( auto& value : value_arr.data )
        {
            if ( retval || didbrk )
                break;
            didcon = false;

            auto pop_handler = stack.push();
            stack.set( node.var_id, value );
            handle_scope( node.scope, retval, &didbrk, &didcon );
        }
    }
    else
        ENGINE_PANIC( node.location, "can not for loop [", value_type, "]" );
}

dawn::Value const& dawn::Engine::handle_lambda_node( LambdaNode const& node )
{
    auto& func_val = node.func_value;
    func_val.as_function().as_lambda().frame = stack.peek();
    return func_val;
}

dawn::Value dawn::Engine::handle_enum_node( EnumNode const& node )
{
    auto* enum_ptr = global.enums.get( node.type_id );
    if ( !enum_ptr )
        ENGINE_PANIC( node.location, "enum [", IDSystem::get( node.type_id ), "] does not exist" );

    auto* entry_ptr = enum_ptr->get( node.key_id );
    if ( !entry_ptr )
        ENGINE_PANIC( node.location, "enum [", IDSystem::get( node.type_id ), "] does not have key [", IDSystem::get( node.key_id ), "]" );
    if ( !std::holds_alternative<Holder<Value>>( entry_ptr->expr ) )
        ENGINE_PANIC( node.location, "value expected at enum [", IDSystem::get( node.type_id ), "] and key [", IDSystem::get( node.key_id ), "] but found node" );

    EnumValue result{};
    result.parent_id = node.type_id;
    result.key_id = node.key_id;
    result.value = std::get<Holder<Value>>( entry_ptr->expr );

    return Value{ result, node.location };
}

dawn::Value dawn::Engine::handle_struct_node( StructNode const& node )
{
    auto* struct_ptr = global.structs.get( node.type_id );
    if ( !struct_ptr )
        ENGINE_PANIC( node.location, "struct [", IDSystem::get( node.type_id ), "] does not exist" );
    auto& struc = *struct_ptr;

    Value value{ StructValue{ node.type_id }, node.location };
    auto& struc_value = const_cast<StructValue&>( value.as_struct() ); // This is fine because parent_id was already passed in the constructor, line above.
    struc_value.members.reserve( struc.fields.size() + struc.methods.size() );

    // Structure default initialization.
    for ( auto& field : struc.fields )
        struc_value.members[field.id] = { .value = create_default_value( this, field.type_id, node.location ), .type = MemberType::FIELD };

    // Structure argument initialization.
    if ( std::holds_alternative<StructNode::NamedInit>( node.init ) )
    {
        auto& args = std::get<StructNode::NamedInit>( node.init ).args;
        for ( auto& [id, arg_node] : args )
        {
            auto field_it = struc_value.members.find( id );
            if ( field_it == struc_value.members.end() ) // Only fields are stored at this stage.
                ENGINE_PANIC( node.location, "struct [", IDSystem::get( struc.id ), "] does not contain field [", IDSystem::get( id ), "]" );

            auto& field = field_it->second;
            Value expr = handle_expr( arg_node ).clone();
            if ( field.value.type() != expr.type() )
                ENGINE_PANIC( node.location, "can not assign type [", expr.type(), "] to type [", field.value.type(), "]" );

            field.value = expr; // Must use = instead of assign() because member is const at this stage.
        }
    }
    else
    {
        auto& args = std::get<StructNode::ListInit>( node.init ).args;
        if ( args.size() > struc.fields.size() )
            ENGINE_PANIC( node.location, "struct [", IDSystem::get( struc.id ), "] contains [", struc.fields.size(), "] fields but [", args.size(), "] were passed" );

        for ( Int i = 0; i < (Int) args.size(); i++ )
        {
            auto& field = struc_value.members[struc.fields[i].id];
            Value expr = handle_expr( args[i] ).clone();
            if ( field.value.type() != expr.type() )
                ENGINE_PANIC( node.location, "can not assign type [", expr.type(), "] to type [", field.value.type(), "]" );

            field = { .value = expr, .type = MemberType::FIELD }; // Must use = instead of assign() because member is const at this stage.
        }
    }

    // Structure methods.
    for ( auto& method : struc.methods )
    {
        FunctionValue fv{};
        auto& f = fv.data.emplace<FunctionValue::AsMethod>();
        f.id = method.id;
        f.func = DFunction{ method.args, method.body };
        *f.self = value;
        struc_value.members[f.id] = { .value = Value{ fv, node.location }, .type = MemberType::METHOD };
    }

    return value;
}

dawn::Value dawn::Engine::handle_array_node( ArrayNode const& node )
{
    ArrayValue result{};
    if ( std::holds_alternative<ArrayNode::ListInit>( node.init ) )
    {
        auto& init_data = std::get<ArrayNode::ListInit>( node.init );
        result.data.reserve( init_data.elements.size() );
        for ( auto& expr : init_data.elements )
            result.data.push_back( handle_expr( expr ).clone() );
    }
    else
    {
        auto& init_data = std::get<ArrayNode::SizedInit>( node.init );
        Int size = handle_expr( *init_data.size_expr ).as_int();
        if ( size < 0 )
            ENGINE_PANIC( node.location, "array size can not be negative" );
        result.data.reserve( size );
        for ( Int i = 0; i < size; i++ )
            result.data.push_back( create_default_value( this, init_data.type_id, node.location ) );
    }
    return Value{ result, node.location };
}

dawn::Value dawn::Engine::handle_ac_node( AccessNode const& node )
{
    const Value left = handle_expr( *node.left_expr );
    if ( left.type() == ValueType::STRUCT )
    {
        auto& left_struct = left.as_struct();
        auto it = left_struct.members.find( node.right_id );
        if ( it == left_struct.members.end() )
            ENGINE_PANIC( node.location, "struct [", IDSystem::get( left_struct.parent_id ), "] does not have member [", IDSystem::get( node.right_id ), "]" );
        return it->second.value;
    }
    else
    {
        auto* generator_ptr = global.member_generators[(Int) left.type()].get( node.right_id );
        if ( !generator_ptr )
            ENGINE_PANIC( node.location, "type [", left.type(), "] does not have member [", IDSystem::get( node.right_id ), "]" );
        return ( *generator_ptr )( node.location, *this, left );
    }
}

dawn::Value dawn::Engine::handle_op_node( OperatorNode const& node )
{
    const Value left = handle_expr( node.sides[0] ); // Make sure that left side is computed first.
    return handle_oper( node.location, left, node.type, handle_expr( node.sides[1] ) );
}

dawn::Value dawn::Engine::handle_as_node( AssignNode const& node )
{
    Value left = handle_expr( node.sides[0] );
    switch ( node.type )
    {
    case AssignType::ASSIGN:
        left.assign( handle_expr( node.sides[1] ) );
        return left;

    case AssignType::ADD:
        left.assign( handle_oper( node.location, left, OperatorType::ADD, handle_expr( node.sides[1] ) ) );
        return left;

    case AssignType::SUB:
        left.assign( handle_oper( node.location, left, OperatorType::SUB, handle_expr( node.sides[1] ) ) );
        return left;

    case AssignType::MUL:
        left.assign( handle_oper( node.location, left, OperatorType::MUL, handle_expr( node.sides[1] ) ) );
        return left;

    case AssignType::DIV:
        left.assign( handle_oper( node.location, left, OperatorType::DIV, handle_expr( node.sides[1] ) ) );
        return left;

    case AssignType::POW:
        left.assign( handle_oper( node.location, left, OperatorType::POW, handle_expr( node.sides[1] ) ) );
        return left;

    case AssignType::MOD:
        left.assign( handle_oper( node.location, left, OperatorType::MOD, handle_expr( node.sides[1] ) ) );
        return left;

    default:
        ENGINE_PANIC( node.location, "unknown assign node type: ", (Int) node.type );
    }
}

void dawn::Engine::add_var( Location const& location, VarType const& type, ID id, Value const& value )
{
    if ( type.type_id != value.type_id() )
        ENGINE_PANIC( location, "can not init variable of type [", IDSystem::get( type.type_id ), "] with type [", IDSystem::get( value.type_id() ), "]" );

    if ( type.kind == VarKind::CONSTANT )
        stack.set( id, value.clone() );
    else if ( type.kind == VarKind::VARIABLE )
        stack.set( id, value.clone().unlock_const() );
    else
        stack.set( id, value );
}

void dawn::Engine::handle_scope( Scope const& scope, Opt<Value>& retval, Bool* didbrk, Bool* didcon )
{
    for ( auto& instr : scope.instr )
    {
        if ( retval || ( didbrk && *didbrk ) || ( didcon && *didcon ) )
            break;

        handle_instr( instr, retval, didbrk, didcon );
    }
}
