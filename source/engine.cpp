#include "engine.h"


dawn::Engine::Engine()
{
    load_standard_functions();
    load_standard_members();
}

void dawn::Engine::load_mod( Module const& module )
{
    for ( auto& entry : module.functions )
        load_function( entry );

    for ( auto& entry : module.enums )
        load_enum( entry );

    for ( auto& entry : module.structs )
        load_struct( entry );

    for ( auto& entry : module.variables )
        load_variable( entry );
}

void dawn::Engine::load_function( Function const& entry )
{
    if ( stack.root().get( entry.id ) )
        ENGINE_PANIC( Location::none, "object [", IDSystem::get( entry.id ), "] already exists" );

    FunctionValue fv{};
    auto& global = fv.data.emplace<FunctionValue::AsGlobal>();
    global.id = entry.id;
    global.func = DFunction{ entry.args, entry.body };

    stack.root().set( entry.id, Value{ fv } );
}

void dawn::Engine::load_enum( Enum const& entry )
{
    Enum enu = entry;
    for ( auto& entry : enu.entries )
    {
        if ( !std::holds_alternative<NodeRef>( entry.expr ) )
            continue;
        auto const& expr = std::get<NodeRef>( entry.expr ).value();
        *entry.expr.emplace<Holder<Value>>() = handle_expr( expr );
    }
    enums.set( enu.id, enu );
}

void dawn::Engine::load_struct( Struct const& entry )
{
    structs.set( entry.id, entry );
}

void dawn::Engine::load_variable( Variable const& entry )
{
    auto& expr = entry.expr.value();
    add_var( expr.location(), entry.type, entry.id, handle_expr( expr ) );
}

void dawn::Engine::bind_cfunc( Int id, Bool is_ctime, CFunction cfunc )
{
    if ( stack.root().get( id ) )
        ENGINE_PANIC( Location::none, "object [", IDSystem::get( id ), "] already exists" );
    if ( is_ctime )
        m_ctime_funcs.insert( id );

    FunctionValue fv{};
    auto& global = fv.data.emplace<FunctionValue::AsGlobal>();
    global.id = id;
    global.func = std::move( cfunc );
    stack.root().set( id, Value{ fv } );
}

dawn::Value dawn::Engine::call_func( Int id, Value* args, Int arg_count )
{
    Value* value = stack.root().get( id );
    if ( !value )
        ENGINE_PANIC( Location::none, "object [", IDSystem::get( id ), "] does not exist" );

    if ( value->type() != ValueType::FUNCTION )
        ENGINE_PANIC( Location::none, "object [", IDSystem::get( id ), "] can not be called" );

    return handle_func( Location::none, value->as_function(), args, arg_count );
}

void dawn::Engine::add_var( Location const& location, VarType const& type, Int id, Value const& value )
{
    if ( type.type_id != value.type_id() )
        ENGINE_PANIC( location, "can not init variable of type [", IDSystem::get( type.type_id ), "] with type [", IDSystem::get( value.type_id() ), "]" );

    if ( type.kind == VarType::Kind::CONSTANT )
        stack.current().set( id, value.clone() );
    else if ( type.kind == VarType::Kind::VARIABLE )
        stack.current().set( id, value.clone().unlock_const() );
    else
        stack.current().set( id, value );
}

dawn::Value* dawn::Engine::get_var( Int id )
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
    const Int id = IDSystem::get( name );
    member_generators[(Int) type].set( id, [name, is_const, expected_args, body, id]( Location const& _, Engine& __, Value const& self ) -> Value
        {
            FunctionValue fv{};
            auto& method = fv.data.emplace<FunctionValue::AsMethod>();
            method.id = id;
            method.func = [name, is_const, expected_args, body, self]( Location const& location, Engine& engine, Value* args, Int arg_count ) -> Value
                {
                    if ( !is_const && self.is_const() )
                        ENGINE_PANIC( location, "can not call [", name, "] on a const value" );
                    if ( ( 1 + expected_args ) != arg_count )
                        ENGINE_PANIC( location, "method [", name, "] expects self + ", expected_args, " arguments" );
                    return body( location, engine, args[0], args + 1 );
                };
            *method.self = self;
            return (Value) fv;
        } );
}

dawn::Set<dawn::Int> const& dawn::Engine::ctime_funcs() const
{
    return m_ctime_funcs;
}

dawn::Value dawn::Engine::handle_func( Location const& location, FunctionValue const& func, Value* args, Int arg_count )
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
        return handle_value_node( std::get<ValueNode>( node ) );

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

    case NodeType::UNARY:
        return handle_un_node( std::get<UnaryNode>( node ) );

    case NodeType::OPERATOR:
        return handle_op_node( std::get<OperatorNode>( node ) );

    case NodeType::ASSIGN:
        return handle_as_node( std::get<AssignNode>( node ) );

    default:
        ENGINE_PANIC( node.location(), "unknown expr node type: ", (Int) node.type() );
    }
}

dawn::Value dawn::Engine::handle_value_node( ValueNode const& node )
{
    return node.value;
}

void dawn::Engine::handle_var_node( VariableNode const& node )
{
    add_var( node.location, node.var.type, node.var.id, handle_expr( node.var.expr.value() ) );
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
    Value left = handle_expr( node.left_expr.value() );
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
    Value left = handle_expr( node.left_expr.value() );
    Int index = handle_expr( node.expr.value() ).to_int( node.location, *this );

    if ( left.type() == ValueType::STRING )
    {
        auto& value = left.as_string();
        if ( index < 0 || index >= (Int) value.size() )
            ENGINE_PANIC( node.location, "string access [", index, "] out of bounds" );
        return Value{ value[index] };
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
    retval = handle_expr( node.expr.value() );
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
    throw handle_expr( node.expr.value() );
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
        if ( !handle_expr( part.expr ).to_bool( node.location, *this ) )
            continue;

        auto pop_handler = stack.push();
        handle_scope( part.scope, retval, didbrk, didcon );
        break;
    }
}

void dawn::Engine::handle_switch_node( SwitchNode const& node, Opt<Value>& retval, Bool* didbrk, Bool* didcon )
{
    Value check_value = handle_expr( node.main_expr.value() );

    for ( auto& case_part : node.cases )
    {
        for ( auto& expr : case_part.exprs )
        {
            if ( !check_value.op_eq( node.location, *this, handle_expr( expr ) ).to_bool( node.location, *this ) )
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

        if ( !handle_expr( node.expr.value() ).to_bool( node.location, *this ) )
            break;

        auto pop_handler = stack.push();
        handle_scope( node.scope, retval, &didbrk, &didcon );
    }
}

void dawn::Engine::handle_for_node( ForNode const& node, Opt<Value>& retval )
{
    Value loop_value = handle_expr( node.expr.value() );

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
            stack.current().set( node.var_id, Value{ i } );
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
            stack.current().set( node.var_id, Value{ c } );
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

    return Value{ result };
}

dawn::Value dawn::Engine::handle_struct_node( StructNode const& node )
{
    auto* struct_ptr = structs.get( node.type_id );
    if ( !struct_ptr )
        ENGINE_PANIC( node.location, "struct [", IDSystem::get( node.type_id ), "] does not exist" );
    auto& struc = *struct_ptr;

    Value value{ StructValue{} };
    auto& struc_value = value.as_struct();
    struc_value.parent_id = node.type_id;

    // Structure default initialization.
    struc_value.fields.reserve( struc.fields.size() );
    for ( auto& field : struc.fields )
        struc_value.fields[field.id] = create_default_value( node.location, field.type_id );

    // Structure argument initialization.
    if ( std::holds_alternative<StructNode::NamedInit>( node.init ) )
    {
        auto& args = std::get<StructNode::NamedInit>( node.init ).args;
        for ( auto& [id, arg_node] : args )
        {
            auto field_it = struc_value.fields.find( id );
            if ( field_it == struc_value.fields.end() )
                ENGINE_PANIC( node.location, "struct [", IDSystem::get( struc.id ), "] does not contain field [", IDSystem::get( id ), "]" );

            auto& field = field_it->second;
            Value expr = handle_expr( arg_node ).clone();
            if ( field.type() != expr.type() )
                ENGINE_PANIC( node.location, "can not assign type [", expr.type(), "] to type [", field.type(), "]" );

            field = expr; // Must use = instead of assign() because member is const at this stage.
        }
    }
    else
    {
        auto& args = std::get<StructNode::ListInit>( node.init ).args;
        if ( args.size() > struc.fields.size() )
            ENGINE_PANIC( node.location, "struct [", IDSystem::get( struc.id ), "] contains [", struc.fields.size(), "] fields but [", args.size(), "] were passed" );

        for ( Int i = 0; i < (Int) args.size(); i++ )
        {
            auto& field = struc_value.fields[struc.fields[i].id];
            Value expr = handle_expr( args[i] ).clone();
            if ( field.type() != expr.type() )
                ENGINE_PANIC( node.location, "can not assign type [", expr.type(), "] to type [", field.type(), "]" );

            field = expr; // Must use = instead of assign() because member is const at this stage.
        }
    }

    // Structure methods.
    struc_value.methods.reserve( struc.methods.size() );
    for ( auto& method : struc.methods )
    {
        FunctionValue fv{};
        auto& f = fv.data.emplace<FunctionValue::AsMethod>();
        f.id = method.id;
        f.func = DFunction{ method.args, method.body };
        *f.self = value;
        struc_value.methods[f.id] = Value{ fv };
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
        Int size = handle_expr( init_data.size_expr.value() ).to_int( node.location, *this );
        if ( size < 0 )
            ENGINE_PANIC( node.location, "array size can not be negative" );
        result.data.reserve( size );
        for ( Int i = 0; i < size; i++ )
            result.data.push_back( create_default_value( node.location, init_data.type_id ) );
    }
    return Value{ result };
}

dawn::Value dawn::Engine::handle_un_node( UnaryNode const& node )
{
    switch ( node.type )
    {
    case UnaryType::PLUS:
        return Value{ handle_expr( node.right.value() ).un_plus( node.location, *this ) };

    case UnaryType::MINUS:
        return Value{ handle_expr( node.right.value() ).un_minus( node.location, *this ) };

    case UnaryType::NOT:
        return Value{ handle_expr( node.right.value() ).un_not( node.location, *this ) };

    default:
        ENGINE_PANIC( node.location, "unknown unary node type: ", typeid( node ).name() );
    }
}

dawn::Value dawn::Engine::handle_op_node( OperatorNode const& node )
{
    switch ( node.type )
    {
    case OperatorType::ACCESS:
        return handle_ac_node( node );

    case OperatorType::POW:
        return handle_expr( node.sides[0] ).op_pow( node.location, *this, handle_expr( node.sides[1] ) );

    case OperatorType::MOD:
        return handle_expr( node.sides[0] ).op_mod( node.location, *this, handle_expr( node.sides[1] ) );

    case OperatorType::MUL:
        return handle_expr( node.sides[0] ).op_mul( node.location, *this, handle_expr( node.sides[1] ) );

    case OperatorType::DIV:
        return handle_expr( node.sides[0] ).op_div( node.location, *this, handle_expr( node.sides[1] ) );

    case OperatorType::ADD:
        return handle_expr( node.sides[0] ).op_add( node.location, *this, handle_expr( node.sides[1] ) );

    case OperatorType::SUB:
        return handle_expr( node.sides[0] ).op_sub( node.location, *this, handle_expr( node.sides[1] ) );

    case OperatorType::COMPARE:
        return handle_expr( node.sides[0] ).op_cmpr( node.location, *this, handle_expr( node.sides[1] ) );

    case OperatorType::LESS:
        return handle_expr( node.sides[0] ).op_less( node.location, *this, handle_expr( node.sides[1] ) );

    case OperatorType::GREAT:
        return handle_expr( node.sides[0] ).op_great( node.location, *this, handle_expr( node.sides[1] ) );

    case OperatorType::LESS_EQ:
        return handle_expr( node.sides[0] ).op_lesseq( node.location, *this, handle_expr( node.sides[1] ) );

    case OperatorType::GREAT_EQ:
        return handle_expr( node.sides[0] ).op_greateq( node.location, *this, handle_expr( node.sides[1] ) );

    case OperatorType::EQ:
        return handle_expr( node.sides[0] ).op_eq( node.location, *this, handle_expr( node.sides[1] ) );

    case OperatorType::NOT_EQ:
        return handle_expr( node.sides[0] ).op_neq( node.location, *this, handle_expr( node.sides[1] ) );

    case OperatorType::AND:
        return handle_expr( node.sides[0] ).op_and( node.location, *this, handle_expr( node.sides[1] ) );

    case OperatorType::OR:
        return handle_expr( node.sides[0] ).op_or( node.location, *this, handle_expr( node.sides[1] ) );

    case OperatorType::RANGE:
        return handle_expr( node.sides[0] ).op_range( node.location, *this, handle_expr( node.sides[1] ) );

    default:
        ENGINE_PANIC( node.location, "unknown operator node type: ", typeid( node ).name() );
    }
}

dawn::Value dawn::Engine::handle_ac_node( OperatorNode const& node )
{
    Value left = handle_expr( node.sides[0] );

    if ( node.sides[1].type() != NodeType::IDENTIFIER )
        ENGINE_PANIC( node.location, "access must be an identifier" );

    Int right_id = std::get<IdentifierNode>( node.sides[1] ).id;
    if ( left.type() == ValueType::STRUCT )
        return handle_ac_struct_node( node.location, left, right_id );
    else
        return handle_ac_type_node( node.location, left, right_id );
}

dawn::Value dawn::Engine::handle_as_node( AssignNode const& node )
{
    Value left = handle_expr( node.sides[0] );

    switch ( node.type )
    {
    case AssignType::ASSIGN:
        left.assign( node.location, handle_expr( node.sides[1] ) );
        return left;

    case AssignType::ADD:
        left.assign( node.location, left.op_add( node.location, *this, handle_expr( node.sides[1] ) ) );
        return left;

    case AssignType::SUB:
        left.assign( node.location, left.op_sub( node.location, *this, handle_expr( node.sides[1] ) ) );
        return left;

    case AssignType::MUL:
        left.assign( node.location, left.op_mul( node.location, *this, handle_expr( node.sides[1] ) ) );
        return left;

    case AssignType::DIV:
        left.assign( node.location, left.op_div( node.location, *this, handle_expr( node.sides[1] ) ) );
        return left;

    case AssignType::POW:
        left.assign( node.location, left.op_pow( node.location, *this, handle_expr( node.sides[1] ) ) );
        return left;

    case AssignType::MOD:
        left.assign( node.location, left.op_mod( node.location, *this, handle_expr( node.sides[1] ) ) );
        return left;

    default:
        ENGINE_PANIC( node.location, "unknown assign node type: ", typeid( node ).name() );
    }
}

dawn::Value dawn::Engine::handle_ac_struct_node( Location const& location, Value const& self, Int right_id )
{
    auto& left = self.as_struct();
    auto it = left.fields.find( right_id );
    if ( it != left.fields.end() )
        return it->second;
    it = left.methods.find( right_id );
    if ( it != left.methods.end() )
        return it->second;
    ENGINE_PANIC( location, "struct [", IDSystem::get( left.parent_id ), "] does not have member [", IDSystem::get( right_id ), "]" );
}

dawn::Value dawn::Engine::handle_ac_type_node( Location const& location, Value const& self, Int right_id )
{
    auto* generator_ptr = member_generators[(Int) self.type()].get( right_id );
    if ( !generator_ptr )
        ENGINE_PANIC( location, "type [", self.type(), "] does not have member [", IDSystem::get( right_id ), "]" );
    return ( *generator_ptr )( location, *this, self );
}

dawn::Value dawn::Engine::create_default_value( Location const& location, Int typeid_ )
{
    static const Int _id_nothing = IDSystem::get( tp_nothing );
    static const Int _id_bool = IDSystem::get( tp_bool );
    static const Int _id_int = IDSystem::get( tp_int );
    static const Int _id_float = IDSystem::get( tp_float );
    static const Int _id_char = IDSystem::get( tp_char );
    static const Int _id_string = IDSystem::get( tp_string );
    static const Int _id_function = IDSystem::get( tp_function );
    static const Int _id_array = IDSystem::get( tp_array );
    static const Int _id_range = IDSystem::get( tp_range );

    if ( typeid_ == _id_nothing )
        return Value{ Nothing{} };

    else if ( typeid_ == _id_bool )
        return Value{ Bool{} };

    else if ( typeid_ == _id_int )
        return Value{ Int{} };

    else if ( typeid_ == _id_float )
        return Value{ Float{} };

    else if ( typeid_ == _id_char )
        return Value{ Char{} };

    else if ( typeid_ == _id_string )
        return Value{ StringRef{} };

    else if ( typeid_ == _id_function )
        return Value{ FunctionValue{} };

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
        return Value{ ArrayValue{} };

    else if ( typeid_ == _id_range )
        return Value{ RangeValue{} };

    else
        ENGINE_PANIC( location, "type [", IDSystem::get( typeid_ ), "] does not exist" );
}
