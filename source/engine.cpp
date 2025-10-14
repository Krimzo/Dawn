#include "engine.h"


dawn::Engine::Engine()
{
    load_standard_operators();
    load_standard_functions();
    load_standard_members();
}

void dawn::Engine::load_mod( Module const& module )
{
    for ( auto& entry : module.operators )
        load_operator( entry );

    for ( auto& entry : module.functions )
        load_function( entry );

    for ( auto& entry : module.enums )
        load_enum( entry );

    for ( auto& entry : module.structs )
        load_struct( entry );

    for ( auto& entry : module.variables )
        load_variable( entry );
}

void dawn::Engine::load_operator( Operator const& entry )
{
    if ( entry.args.size() != 2 )
        ENGINE_PANIC( LOCATION_NONE, "operators must have 2 arguments" );

    auto const& left = entry.args[0];
    auto const& right = entry.args[1];

    auto& left_types = operators[(Int) entry.type];

    auto* right_types = left_types.get( left.type.type_id );
    if ( !right_types )
        right_types = &left_types.set( left.type.type_id, {} );

    auto* op = right_types->get( right.type.type_id );
    if ( op )
        ENGINE_PANIC( LOCATION_NONE, "operator [", entry.type, "] with left type [", IDSystem::get( left.type.type_id ), "] and right type [", IDSystem::get( right.type.type_id ), "] already defined" );
    right_types->set( right.type.type_id, {} ).as_global().func.emplace<DFunction>( entry.args, entry.body );
}

void dawn::Engine::load_function( Function const& entry )
{
    if ( stack.root().get( entry.id ) )
        ENGINE_PANIC( LOCATION_NONE, "object [", IDSystem::get( entry.id ), "] already exists" );

    FunctionValue fv{};
    auto& global = fv.data.emplace<FunctionValue::AsGlobal>();
    global.id = entry.id;
    global.func = DFunction{ entry.args, entry.body };

    stack.root().set( entry.id, Value{ fv, LOCATION_NONE } );
}

void dawn::Engine::load_enum( Enum const& entry )
{
    Enum enu = entry;
    for ( auto& entry : enu.entries )
    {
        if ( auto* expr_refptr = std::get_if<NodeRef>( &entry.expr ) )
            *entry.expr.emplace<Holder<Value>>() = handle_expr( **expr_refptr );
    }
    enums.set( enu.id, enu );
}

void dawn::Engine::load_struct( Struct const& entry )
{
    structs.set( entry.id, entry );
}

void dawn::Engine::load_variable( Variable const& entry )
{
    auto& expr = *entry.expr;
    add_var( expr.location(), entry.type, entry.id, handle_expr( expr ) );
}

void dawn::Engine::bind_oper( ID left_type_id, OperatorType op_type, ID right_type_id, Bool is_const, CFunction cfunc )
{
    auto& left_types = operators[(Int) op_type];

    auto* right_types = left_types.get( left_type_id );
    if ( !right_types )
        right_types = &left_types.set( left_type_id, {} );

    auto* op = right_types->get( right_type_id );
    if ( op )
        ENGINE_PANIC( LOCATION_NONE, "operator [", op_type, "] with left type [", IDSystem::get( left_type_id ), "] and right type [", IDSystem::get( right_type_id ), "] already defined" );
    right_types->set( right_type_id, {} ).as_global().func.emplace<CFunction>( std::move( cfunc ) );

    if ( is_const )
        m_ctime_ops[(Int) op_type].insert( combine_ids( left_type_id, right_type_id ) );
}

void dawn::Engine::bind_func( ID id, Bool is_ctime, CFunction cfunc )
{
    if ( stack.root().get( id ) )
        ENGINE_PANIC( LOCATION_NONE, "object [", IDSystem::get( id ), "] already exists" );
    if ( is_ctime )
        m_ctime_funcs.insert( id );

    FunctionValue fv{};
    auto& global = fv.data.emplace<FunctionValue::AsGlobal>();
    global.id = id;
    global.func = std::move( cfunc );
    stack.root().set( id, Value{ fv, LOCATION_NONE } );
}

dawn::Value dawn::Engine::call_func( ID id, Value* args, Int arg_count )
{
    Value* value = stack.root().get( id );
    if ( !value )
        ENGINE_PANIC( LOCATION_NONE, "object [", IDSystem::get( id ), "] does not exist" );

    if ( value->type() != ValueType::FUNCTION )
        ENGINE_PANIC( LOCATION_NONE, "object [", IDSystem::get( id ), "] can not be called" );

    return handle_func( LOCATION_NONE, value->as_function(), args, arg_count );
}

void dawn::Engine::add_var( Location const& location, VarType const& type, ID id, Value const& value )
{
    if ( type.type_id != value.type_id() )
        ENGINE_PANIC( location, "can not init variable of type [", IDSystem::get( type.type_id ), "] with type [", IDSystem::get( value.type_id() ), "]" );

    if ( type.kind == VarKind::CONSTANT )
        stack.current().set( id, value.clone() );
    else if ( type.kind == VarKind::VARIABLE )
        stack.current().set( id, value.clone().unlock_const() );
    else
        stack.current().set( id, value );
}

dawn::Value* dawn::Engine::get_var( ID id )
{
    return stack.current().get( id );
}

void dawn::Engine::bind_member( ValueType type, StringRef const& name, CustomMemberFunc const& func )
{
    member_generators[(Int) type].set( IDSystem::get( name ), [func]( Location const& location, Engine& engine, Value const& self ) -> Value
        {
            return func( location, engine, const_cast<Value&>( self ) );
        } );
}

void dawn::Engine::bind_method( ValueType type, String const& name, Bool is_const, Int expected_args, CustomMethodFunc const& body )
{
    const ID id = IDSystem::get( name );
    member_generators[(Int) type].set( id, [name, is_const, expected_args, body, id]( Location const& location, Engine& __, Value const& self ) -> Value
        {
            FunctionValue fv{};
            auto& method = fv.data.emplace<FunctionValue::AsMethod>();
            method.id = id;
            method.func = [name, is_const, expected_args, body, self]( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
                {
                    if ( !is_const && self.is_const() )
                        ENGINE_PANIC( location, "can not call [", name, "] on a const value" );
                    if ( ( 1 + expected_args ) != arg_count )
                        ENGINE_PANIC( location, "method [", name, "] expects self + ", expected_args, " arguments" );
                    return body( location, engine, args[0], args + 1 );
                };
            *method.self = self;
            return Value{ fv, location };
        } );
}

dawn::Value dawn::Engine::handle_oper( Location const& location, Value const& left, const OperatorType op_type, Value const& right )
{
    const auto overload_caller = [&]() -> Value
        {
            auto& op_left_ids = operators[(Int) op_type];
            auto* op_right_ids = op_left_ids.get( left.type_id() );
            if ( !op_right_ids )
                ENGINE_PANIC( location, "type [", IDSystem::get( left.type_id() ), "] does not support operator [", op_type, "]" );
            auto* func = op_right_ids->get( right.type_id() );
            if ( !func )
                ENGINE_PANIC( location, "type [", IDSystem::get( left.type_id() ), "] does not support operator [", op_type, "] with right type being [", IDSystem::get( right.type_id() ), "]" );
            Value args[2] = { left, right };
            return handle_func( location, *func, args, (Int) std::size( args ) );
        };

    switch ( op_type )
    {
    case OperatorType::POW:
        return overload_caller();

    case OperatorType::MOD:
        return overload_caller();

    case OperatorType::MUL:
        return overload_caller();

    case OperatorType::DIV:
        return overload_caller();

    case OperatorType::ADD:
        return overload_caller();

    case OperatorType::SUB:
        return overload_caller();

    case OperatorType::COMPARE:
        return overload_caller();

    case OperatorType::LESS:
        return Value{ handle_oper( location, left, OperatorType::COMPARE, right ).as_int() < 0, location };

    case OperatorType::GREAT:
        return Value{ handle_oper( location, left, OperatorType::COMPARE, right ).as_int() > 0, location };

    case OperatorType::LESS_EQ:
        return Value{ handle_oper( location, left, OperatorType::COMPARE, right ).as_int() <= 0, location };

    case OperatorType::GREAT_EQ:
        return Value{ handle_oper( location, left, OperatorType::COMPARE, right ).as_int() >= 0, location };

    case OperatorType::EQ:
        return Value{ handle_oper( location, left, OperatorType::COMPARE, right ).as_int() == 0, location };

    case OperatorType::NOT_EQ:
        return Value{ handle_oper( location, left, OperatorType::COMPARE, right ).as_int() != 0, location };

    case OperatorType::NOT:
        return Value{ !right.as_bool(), location };

    case OperatorType::AND:
        return Value{ left.as_bool() && right.as_bool(), location };

    case OperatorType::OR:
        return Value{ left.as_bool() || right.as_bool(), location };

    case OperatorType::RANGE:
        return Value{ RangeValue{ .start_incl = left.as_int(), .end_excl = right.as_int() }, location };

    default:
        ENGINE_PANIC( location, "unknown operator type [", op_type, "]" );
    }
}

dawn::Value dawn::Engine::handle_func( Location const& location, FunctionValue const& func, Value const* args, Int arg_count )
{
    if ( auto* dfunc = func.dfunction() )
    {
        if ( dfunc->args.size() != arg_count )
        {
            if ( func.is_global() )
                ENGINE_PANIC( location, "invalid argument count for function [", IDSystem::get( func.as_global().id ), "]" );
            else if ( func.is_method() )
                ENGINE_PANIC( location, "invalid argument count for method [", IDSystem::get( func.as_method().id ), "]" );
            else
                ENGINE_PANIC( location, "invalid argument count for lambda" );
        }

        auto pop_handler = stack.push_from(
            func.is_lambda() ? func.as_lambda().frame : RegisterRef<Frame>{} );

        for ( Int i = 0; i < arg_count; i++ )
            add_var( location, dfunc->args[i].type, dfunc->args[i].id, args[i] );

        Opt<Value> retval;
        handle_scope( dfunc->body, retval, nullptr, nullptr );
        return retval ? *retval : Value{};
    }
    else
    {
        auto& cfunc = *func.cfunction();
        return cfunc( location, *this, args, arg_count );
    }
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

void dawn::Engine::handle_instr( Node const& node, Opt<Value>& retval, Bool* didbrk, Bool* didcon )
{
    switch ( node.type() )
    {
    case NodeType::SCOPE:
    {
        auto pop_handler = stack.push();
        handle_scope( std::get<Scope>( node ), retval, didbrk, didcon );
    }
    break;

    case NodeType::VARIABLE:
        handle_var_node( std::get<VariableNode>( node ) );
        break;

    case NodeType::RETURN:
        handle_return_node( std::get<ReturnNode>( node ), retval );
        break;

    case NodeType::BREAK:
        handle_break_node( std::get<BreakNode>( node ), didbrk );
        break;

    case NodeType::CONTINUE:
        handle_continue_node( std::get<ContinueNode>( node ), didcon );
        break;

    case NodeType::THROW:
        handle_throw_node( std::get<ThrowNode>( node ) );
        break;

    case NodeType::TRY:
        handle_try_node( std::get<TryNode>( node ), retval, didbrk, didcon );
        break;

    case NodeType::IF:
        handle_if_node( std::get<IfNode>( node ), retval, didbrk, didcon );
        break;

    case NodeType::SWITCH:
        handle_switch_node( std::get<SwitchNode>( node ), retval, didbrk, didcon );
        break;

    case NodeType::LOOP:
        handle_loop_node( std::get<LoopNode>( node ), retval );
        break;

    case NodeType::WHILE:
        handle_while_node( std::get<WhileNode>( node ), retval );
        break;

    case NodeType::FOR:
        handle_for_node( std::get<ForNode>( node ), retval );
        break;

    default:
        handle_expr( node );
    }
}

dawn::Value dawn::Engine::handle_expr( Node const& node )
{
    switch ( node.type() )
    {
    case NodeType::VALUE:
        return std::get<Value>( node );

    case NodeType::IDENTIFIER:
        return handle_id_node( std::get<IdentifierNode>( node ) );

    case NodeType::CALL:
        return handle_call_node( std::get<CallNode>( node ) );

    case NodeType::INDEX:
        return handle_index_node( std::get<IndexNode>( node ) );

    case NodeType::LAMBDA:
        return handle_lambda_node( std::get<LambdaNode>( node ) );

    case NodeType::ENUM:
        return handle_enum_node( std::get<EnumNode>( node ) );

    case NodeType::STRUCT:
        return handle_struct_node( std::get<StructNode>( node ) );

    case NodeType::ARRAY:
        return handle_array_node( std::get<ArrayNode>( node ) );

    case NodeType::ACCESS:
        return handle_ac_node( std::get<AccessNode>( node ) );

    case NodeType::OPERATOR:
        return handle_op_node( std::get<OperatorNode>( node ) );

    case NodeType::ASSIGN:
        return handle_as_node( std::get<AssignNode>( node ) );

    default:
        ENGINE_PANIC( node.location(), "unknown expr node type: ", (Int) node.type() );
    }
}

void dawn::Engine::handle_var_node( VariableNode const& node )
{
    add_var( node.location, node.var.type, node.var.id, handle_expr( *node.var.expr ) );
}

dawn::Value dawn::Engine::handle_id_node( IdentifierNode const& node )
{
    auto* ptr = get_var( node.id );
    if ( !ptr )
        ENGINE_PANIC( node.location, "object [", IDSystem::get( node.id ), "] does not exist" );
    return *ptr;
}

dawn::Value dawn::Engine::handle_call_node( CallNode const& node )
{
    Value left = handle_expr( *node.left_expr );
    if ( left.type() != ValueType::FUNCTION )
        ENGINE_PANIC( node.location, "can not call [", left.type(), "]" );

    auto& func = left.as_function();
    Int arg_count = func.is_method() ? ( 1 + node.args.size() ) : node.args.size();

    Value* args_ptr = SALLOC( Value, arg_count );
    SAllocManager alloc_manager{ args_ptr, arg_count };

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
        stack.current().set( node.catch_id, value );
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

    if ( loop_value.type() == ValueType::RANGE )
    {
        auto& value_rng = loop_value.as_range();

        Bool didbrk = false, didcon = false;
        for ( Int i = value_rng.start_incl; i < value_rng.end_excl; ++i )
        {
            if ( retval || didbrk )
                break;
            didcon = false;

            auto pop_handler = stack.push();
            stack.current().set( node.var_id, Value{ i, node.location } );
            handle_scope( node.scope, retval, &didbrk, &didcon );
        }
    }
    else if ( loop_value.type() == ValueType::STRING )
    {
        auto& value_str = loop_value.as_string();

        Bool didbrk = false, didcon = false;
        for ( Char c : value_str )
        {
            if ( retval || didbrk )
                break;
            didcon = false;

            auto pop_handler = stack.push();
            stack.current().set( node.var_id, Value{ c, node.location } );
            handle_scope( node.scope, retval, &didbrk, &didcon );
        }
    }
    else if ( loop_value.type() == ValueType::ARRAY )
    {
        auto& value_arr = loop_value.as_array();

        Bool didbrk = false, didcon = false;
        for ( auto& value : value_arr.data )
        {
            if ( retval || didbrk )
                break;
            didcon = false;

            auto pop_handler = stack.push();
            stack.current().set( node.var_id, value );
            handle_scope( node.scope, retval, &didbrk, &didcon );
        }
    }
    else
        ENGINE_PANIC( node.location, "can not for loop [", loop_value.type(), "]" );
}

dawn::Value dawn::Engine::handle_lambda_node( LambdaNode const& node )
{
    node.func_value.as_function().as_lambda().frame = stack.peek();
    return node.func_value;
}

dawn::Value dawn::Engine::handle_enum_node( EnumNode const& node )
{
    auto* enum_ptr = enums.get( node.type_id );
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
    auto* struct_ptr = structs.get( node.type_id );
    if ( !struct_ptr )
        ENGINE_PANIC( node.location, "struct [", IDSystem::get( node.type_id ), "] does not exist" );
    auto& struc = *struct_ptr;

    Value value{ StructValue{}, node.location };
    auto& struc_value = value.as_struct();
    struc_value.parent_id = node.type_id;

    struc_value.members.reserve( struc.fields.size() + struc.methods.size() );

    // Structure default initialization.
    for ( auto& field : struc.fields )
        struc_value.members[field.id] = { .value = create_default_value( node.location, field.type_id ), .type = MemberType::FIELD };

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
            result.data.push_back( create_default_value( node.location, init_data.type_id ) );
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
        auto* generator_ptr = member_generators[(Int) left.type()].get( node.right_id );
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

dawn::Value dawn::Engine::create_default_value( Location const& location, ID typeid_ )
{
    static const ID _id_void = IDSystem::get( tp_void );
    static const ID _id_bool = IDSystem::get( tp_bool );
    static const ID _id_int = IDSystem::get( tp_int );
    static const ID _id_float = IDSystem::get( tp_float );
    static const ID _id_char = IDSystem::get( tp_char );
    static const ID _id_string = IDSystem::get( tp_string );
    static const ID _id_function = IDSystem::get( tp_function );
    static const ID _id_array = IDSystem::get( tp_array );
    static const ID _id_range = IDSystem::get( tp_range );

    if ( typeid_ == _id_void )
        return Value{};

    else if ( typeid_ == _id_bool )
        return Value{ Bool{}, location };

    else if ( typeid_ == _id_int )
        return Value{ Int{}, location };

    else if ( typeid_ == _id_float )
        return Value{ Float{}, location };

    else if ( typeid_ == _id_char )
        return Value{ Char{}, location };

    else if ( typeid_ == _id_string )
        return Value{ StringRef{}, location };

    else if ( typeid_ == _id_function )
        return Value{ FunctionValue{}, location };

    else if ( auto* enum_ptr = enums.get( typeid_ ) )
    {
        auto& entry = *enum_ptr->entries.begin();
        EnumNode node{ location };
        node.type_id = typeid_;
        node.key_id = entry.id;
        return handle_enum_node( node );
    }

    else if ( auto* struct_ptr = structs.get( typeid_ ) )
    {
        StructNode node{ location };
        node.type_id = typeid_;
        return handle_struct_node( node );
    }

    else if ( typeid_ == _id_array )
        return Value{ ArrayValue{}, location };

    else if ( typeid_ == _id_range )
        return Value{ RangeValue{}, location };

    else
        ENGINE_PANIC( location, "type [", IDSystem::get( typeid_ ), "] does not exist" );
}
