#include "engine.h"


dawn::Engine::Engine()
{
    load_standard_functions();
    load_nothing_members();
    load_bool_members();
    load_int_members();
    load_float_members();
    load_char_members();
    load_string_members();
    load_function_members();
    load_enum_members();
    load_array_members();
    load_range_members();
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
        ENGINE_PANIC( "object [", IDSystem::get( entry.id ), "] already exists" );
    stack.root().set( entry.id, ValueRef{ entry } );
}

void dawn::Engine::load_enum( Enum const& entry )
{
    auto& enu = (enums[entry.id] = entry);
    for ( auto& [key, expr] : enu.keys_expr )
        enu.keys_value[key] = ValueRef{ handle_expr( expr ).value() };
}

void dawn::Engine::load_struct( Struct const& entry )
{
    structs[entry.id] = entry;
}

void dawn::Engine::load_variable( Variable const& entry )
{
    add_var( entry.kind, entry.id, handle_expr( entry.expr.value() ) );
}

void dawn::Engine::bind_func( Int id, Function::CppFunc cpp_func )
{
    Function func;
    func.id = id;
    func.body.emplace<Function::CppFunc>( std::move( cpp_func ) );
    load_function( func );
}

dawn::ValueRef dawn::Engine::call_func( Int id, ValueRef const* args, Int arg_count )
{
    ValueRef* val = stack.root().get( id );
    if ( !val )
        ENGINE_PANIC( "object [", IDSystem::get( id ), "] doesn't exist" );

    if ( val->type() != ValueType::FUNCTION )
        ENGINE_PANIC( "object [", IDSystem::get( id ), "] can't be called" );

    return handle_func( val->as<Function>(), args, arg_count );
}

void dawn::Engine::add_var( VariableKind kind, Int id, ValueRef const& value )
{
    if ( kind == VariableKind::LET )
    {
        stack.current().set( id, ValueRef{ value.value() } );
    }
    else if ( kind == VariableKind::VAR )
    {
        stack.current().set( id, ValueRef{ value.value(), ValueKind::VAR } );
    }
    else
    {
        stack.current().set( id, value );
    }
}

dawn::ValueRef* dawn::Engine::get_var( Int id )
{
    return stack.current().get( id );
}

dawn::ValueRef dawn::Engine::handle_func( Function const& func, ValueRef const* args, Int arg_count )
{
    if ( func.body.index() == 0 )
    {
        if ( func.args.size() != arg_count )
        {
            if ( func.is_lambda() )
                ENGINE_PANIC( "invalid argument count for [lambda]" );
            else
                ENGINE_PANIC( "invalid argument count for function [", IDSystem::get( func.id ), "]" );
        }

        auto pop_handler = stack.push( func );

        for ( Int i = 0; i < arg_count; i++ )
            add_var( func.args[i].kind, func.args[i].id, args[i] );

        ValueRef retval;
        handle_scope( std::get<Scope>( func.body ), retval, nullptr, nullptr );
        return retval ? retval : ValueRef{ Value{} };
    }
    else
    {
        return std::get<Function::CppFunc>( func.body )(args, arg_count);
    }
}

void dawn::Engine::handle_scope( Scope const& scope, ValueRef& retval, Bool* didbrk, Bool* didcon )
{
    for ( auto& instr : scope.instr )
    {
        if ( retval || (didbrk && *didbrk) || (didcon && *didcon) )
            break;

        handle_instr( instr, retval, didbrk, didcon );
    }
}

void dawn::Engine::handle_instr( Node const& node, ValueRef& retval, Bool* didbrk, Bool* didcon )
{
    switch ( node.type() )
    {
    case NodeType::SCOPE:
    {
        auto pop_handler = stack.push();
        handle_scope( node.as<Scope>(), retval, didbrk, didcon );
    }
    break;

    case NodeType::VARIABLE:
        handle_var_node( node.as<VariableNod>() );
        break;

    case NodeType::RETURN:
        handle_return_node( node.as<ReturnNod>(), retval );
        break;

    case NodeType::BREAK:
        handle_break_node( node.as<BreakNod>(), didbrk );
        break;

    case NodeType::CONTINUE:
        handle_continue_node( node.as<ContinueNod>(), didcon );
        break;

    case NodeType::THROW:
        handle_throw_node( node.as<ThrowNod>() );
        break;

    case NodeType::TRY:
        handle_try_node( node.as<TryNod>(), retval, didbrk, didcon );
        break;

    case NodeType::IF:
        handle_if_node( node.as<IfNod>(), retval, didbrk, didcon );
        break;

    case NodeType::SWITCH:
        handle_switch_node( node.as<SwitchNod>(), retval, didbrk, didcon );
        break;

    case NodeType::LOOP:
        handle_loop_node( node.as<LoopNod>(), retval );
        break;

    case NodeType::WHILE:
        handle_while_node( node.as<WhileNod>(), retval );
        break;

    case NodeType::FOR:
        handle_for_node( node.as<ForNod>(), retval );
        break;

    default:
        handle_expr( node );
    }
}

dawn::ValueRef dawn::Engine::handle_expr( Node const& node )
{
    switch ( node.type() )
    {
    case NodeType::REF:
        return handle_ref_node( node.as<RefNod>() );

    case NodeType::IDENTIFIER:
        return handle_id_node( node.as<IdentifierNod>() );

    case NodeType::CALL:
        return handle_call_node( node.as<CallNod>() );

    case NodeType::INDEX:
        return handle_index_node( node.as<IndexNod>() );

    case NodeType::ENUM:
        return handle_enum_node( node.as<EnumNod>() );

    case NodeType::STRUCT:
        return handle_struct_node( node.as<StructNod>() );

    case NodeType::ARRAY:
        return handle_array_node( node.as<ArrayNod>() );

    case NodeType::UNARY:
        return handle_un_node( node.as<UnaryNod>() );

    case NodeType::OPERATOR:
        return handle_op_node( node.as<OperatorNod>() );

    case NodeType::ASSIGN:
        return handle_as_node( node.as<AssignNod>() );

    default:
        ENGINE_PANIC( "unknown expr node type: ", (Int) node.type() );
    }
}

dawn::ValueRef dawn::Engine::handle_ref_node( RefNod const& node )
{
    if ( node.value_ref.type() == ValueType::FUNCTION )
    {
        auto& func = node.value_ref.as<Function>();
        if ( func.is_lambda() )
            func.lambda_parent = stack.peek();
    }
    return node.value_ref;
}

void dawn::Engine::handle_var_node( VariableNod const& node )
{
    add_var( node.var.kind, node.var.id, handle_expr( node.var.expr.value() ) );
}

dawn::ValueRef dawn::Engine::handle_id_node( IdentifierNod const& node )
{
    auto* ptr = get_var( node.id );
    if ( !ptr )
        ENGINE_PANIC( "object [", IDSystem::get( node.id ), "] doesn't exist" );
    return *ptr;
}

dawn::ValueRef dawn::Engine::handle_call_node( CallNod const& node )
{
    ValueRef left_val = handle_expr( node.left_expr.value() );
    if ( left_val.type() != ValueType::FUNCTION )
        ENGINE_PANIC( "can't call [", left_val.type(), "]" );

    auto& func = left_val.as<Function>();
    Int arg_count = func.is_method() ? (1 + node.args.size()) : node.args.size();
    ValueRef* args_ptr = (ValueRef*) _alloca( arg_count * sizeof( ValueRef ) );

    struct SAllocManager
    {
        ValueRef* ptr;
        Int count;

        SAllocManager( ValueRef* ptr, Int count )
            : ptr( ptr ), count( count )
        {
            for ( Int i = 0; i < count; i++ )
                new (ptr + i) ValueRef();
        }

        ~SAllocManager() noexcept
        {
            for ( Int i = 0; i < count; i++ )
                ptr[i].~ValueRef();
        }

        SAllocManager( SAllocManager const& ) = delete;
        void operator=( SAllocManager const& ) = delete;
    };

    SAllocManager alloc_manager{ args_ptr, arg_count };

    if ( func.is_method() )
    {
        args_ptr[0] = func.self_vals[0];
        for ( Int i = 0; i < (Int) node.args.size(); i++ )
            args_ptr[1 + i] = handle_expr( node.args[i] );
    }
    else
    {
        for ( Int i = 0; i < (Int) node.args.size(); i++ )
            args_ptr[i] = handle_expr( node.args[i] );
    }

    return handle_func( func, args_ptr, arg_count );
}

dawn::ValueRef dawn::Engine::handle_index_node( IndexNod const& node )
{
    ValueRef left_val = handle_expr( node.left_expr.value() );
    ValueRef expr_val = handle_expr( node.expr.value() );
    Int index = expr_val.to_int( *this );

    if ( left_val.type() == ValueType::STRING )
    {
        auto& val = left_val.as<String>();
        if ( index < 0 || index >= (Int) val.size() )
            ENGINE_PANIC( "string access [", index, "] out of bounds" );
        return ValueRef{ val[index] };
    }
    else if ( left_val.type() == ValueType::ARRAY )
    {
        auto& val = left_val.as<ArrayVal>();
        if ( index < 0 || index >= (Int) val.data.size() )
            ENGINE_PANIC( "array access [", index, "] out of bounds" );
        return val.data[index];
    }
    else
        ENGINE_PANIC( "can't index type [", left_val.type(), "]" );
}

void dawn::Engine::handle_return_node( ReturnNod const& node, ValueRef& retval )
{
    retval = handle_expr( node.expr.value() );
}

void dawn::Engine::handle_break_node( BreakNod const& node, Bool* didbrk )
{
    if ( !didbrk )
        ENGINE_PANIC( "break statement outside of loop" );
    *didbrk = true;
}

void dawn::Engine::handle_continue_node( ContinueNod const& node, Bool* didcon )
{
    if ( !didcon )
        ENGINE_PANIC( "continue statement outside of loop" );
    *didcon = true;
}

void dawn::Engine::handle_throw_node( ThrowNod const& node )
{
    throw handle_expr( node.expr.value() );
}

void dawn::Engine::handle_try_node( TryNod const& node, ValueRef& retval, Bool* didbrk, Bool* didcon )
{
    try
    {
        auto pop_handler = stack.push();
        handle_scope( node.try_scope, retval, didbrk, didcon );
    }
    catch ( ValueRef const& val )
    {
        auto pop_handler = stack.push();
        add_var( VariableKind::REF, node.catch_id, val );
        handle_scope( node.catch_scope, retval, didbrk, didcon );
    }
}

void dawn::Engine::handle_if_node( IfNod const& node, ValueRef& retval, Bool* didbrk, Bool* didcon )
{
    for ( auto& part : node.parts )
    {
        if ( !handle_expr( part.expr ).to_bool( *this ) )
            continue;

        auto pop_handler = stack.push();
        handle_scope( part.scope, retval, didbrk, didcon );
        break;
    }
}

void dawn::Engine::handle_switch_node( SwitchNod const& node, ValueRef& retval, Bool* didbrk, Bool* didcon )
{
    ValueRef check_val = handle_expr( node.main_expr.value() );

    for ( auto& case_part : node.cases )
    {
        for ( auto& expr : case_part.exprs )
        {
            if ( !check_val.op_eq( *this, handle_expr( expr ) ).to_bool( *this ) )
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

void dawn::Engine::handle_loop_node( LoopNod const& node, ValueRef& retval )
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

void dawn::Engine::handle_while_node( WhileNod const& node, ValueRef& retval )
{
    Bool didbrk = false, didcon = false;
    while ( true )
    {
        if ( retval || didbrk )
            break;
        didcon = false;

        if ( !handle_expr( node.expr.value() ).to_bool( *this ) )
            break;

        auto pop_handler = stack.push();
        handle_scope( node.scope, retval, &didbrk, &didcon );
    }
}

void dawn::Engine::handle_for_node( ForNod const& node, ValueRef& retval )
{
    ValueRef loop_val = handle_expr( node.expr.value() );

    if ( loop_val.type() == ValueType::RANGE )
    {
        auto& value_rng = loop_val.as<RangeVal>();

        Bool didbrk = false, didcon = false;
        for ( Int i = value_rng.start_incl; i < value_rng.end_excl; ++i )
        {
            if ( retval || didbrk )
                break;
            didcon = false;

            auto pop_handler = stack.push();
            add_var( node.var.kind, node.var.id, ValueRef{ i } );
            handle_scope( node.scope, retval, &didbrk, &didcon );
        }
    }
    else if ( loop_val.type() == ValueType::STRING )
    {
        auto& value_str = loop_val.as<String>();

        Bool didbrk = false, didcon = false;
        for ( Char c : value_str )
        {
            if ( retval || didbrk )
                break;
            didcon = false;

            auto pop_handler = stack.push();
            add_var( node.var.kind, node.var.id, ValueRef{ c } );
            handle_scope( node.scope, retval, &didbrk, &didcon );
        }
    }
    else if ( loop_val.type() == ValueType::ARRAY )
    {
        auto& value_arr = loop_val.as<ArrayVal>();

        Bool didbrk = false, didcon = false;
        for ( auto& value : value_arr.data )
        {
            if ( retval || didbrk )
                break;
            didcon = false;

            auto pop_handler = stack.push();
            add_var( node.var.kind, node.var.id, value );
            handle_scope( node.scope, retval, &didbrk, &didcon );
        }
    }
    else
        ENGINE_PANIC( "can't for loop [", loop_val.type(), "]" );
}

dawn::ValueRef dawn::Engine::handle_enum_node( EnumNod const& node )
{
    auto enum_it = enums.find( node.type_id );
    if ( enum_it == enums.end() )
        ENGINE_PANIC( "enum [", IDSystem::get( node.type_id ), "] doesn't exist" );

    if ( !enum_it->second.keys_value.contains( node.key_id ) )
        ENGINE_PANIC( "enum [", IDSystem::get( node.type_id ), "] doesn't have key [", IDSystem::get( node.key_id ), "]" );

    EnumVal result{};
    result.parent = &enum_it->second;
    result.key_id = node.key_id;

    return ValueRef{ result };
}

dawn::ValueRef dawn::Engine::handle_struct_node( StructNod const& node )
{
    auto struct_it = structs.find( node.type_id );
    if ( struct_it == structs.end() )
        ENGINE_PANIC( "struct [", IDSystem::get( node.type_id ), "] doesn't exist" );

    ValueRef value{ StructVal{} };
    auto& result = value.as<StructVal>();
    result.parent = &struct_it->second;

    for ( auto& field : struct_it->second.fields )
    {
        auto arg_it = std::find_if( node.args.begin(), node.args.end(), [&]( auto& arg ) { return arg.first == field.id; } );
        if ( arg_it != node.args.end() )
            continue;

        result.members[field.id] = ValueRef{ handle_expr( field.expr.value() ).value() };
    }

    for ( auto& [id, arg] : node.args )
    {
        auto field_it = std::find_if( struct_it->second.fields.begin(), struct_it->second.fields.end(), [&]( auto& field ) { return field.id == id; } );
        if ( field_it == struct_it->second.fields.end() )
            ENGINE_PANIC( "struct [", IDSystem::get( node.type_id ), "] doesn't contain member [", IDSystem::get( id ), "]" );

        result.members[id] = ValueRef{ handle_expr( arg ).value() };
    }

    for ( auto& method : struct_it->second.methods )
        result.members[method.id] = ValueRef{ method };

    return value;
}

dawn::ValueRef dawn::Engine::handle_array_node( ArrayNod const& node )
{
    ArrayVal result{};

    if ( node.init_type == ArrayNod::InitType::SIZE )
    {
        Int size = handle_expr( node.SIZE_size_expr.value() ).to_int( *this );
        if ( size < 0 )
            ENGINE_PANIC( "array size can't be negative" );

        ValueRef value_val = handle_expr( node.SIZE_value_expr.value() );

        result.data.resize( size );
        for ( auto& val : result.data )
            val = ValueRef{ value_val.value() };
    }
    else
    {
        result.data.reserve( node.LIST_list.size() );
        for ( auto& expr : node.LIST_list )
            result.data.emplace_back( handle_expr( expr ).value() );
    }

    return ValueRef{ result };
}

dawn::ValueRef dawn::Engine::handle_un_node( UnaryNod const& node )
{
    switch ( node.type )
    {
    case UnaryType::PLUS:
        return ValueRef{ handle_expr( node.right.value() ).un_plus( *this ) };

    case UnaryType::MINUS:
        return ValueRef{ handle_expr( node.right.value() ).un_minus( *this ) };

    case UnaryType::NOT:
        return ValueRef{ handle_expr( node.right.value() ).un_not( *this ) };

    default:
        ENGINE_PANIC( "unknown unary node type: ", typeid(node).name() );
    }
}

dawn::ValueRef dawn::Engine::handle_op_node( OperatorNod const& node )
{
    switch ( node.type )
    {
    case OperatorType::ACCESS:
        return handle_ac_node( node );

    case OperatorType::POW:
        return handle_expr( node.sides[0] ).op_pow( *this, handle_expr( node.sides[1] ) );

    case OperatorType::MOD:
        return handle_expr( node.sides[0] ).op_mod( *this, handle_expr( node.sides[1] ) );

    case OperatorType::MUL:
        return handle_expr( node.sides[0] ).op_mul( *this, handle_expr( node.sides[1] ) );

    case OperatorType::DIV:
        return handle_expr( node.sides[0] ).op_div( *this, handle_expr( node.sides[1] ) );

    case OperatorType::ADD:
        return handle_expr( node.sides[0] ).op_add( *this, handle_expr( node.sides[1] ) );

    case OperatorType::SUB:
        return handle_expr( node.sides[0] ).op_sub( *this, handle_expr( node.sides[1] ) );

    case OperatorType::COMPARE:
        return handle_expr( node.sides[0] ).op_cmpr( *this, handle_expr( node.sides[1] ) );

    case OperatorType::LESS:
        return handle_expr( node.sides[0] ).op_less( *this, handle_expr( node.sides[1] ) );

    case OperatorType::GREAT:
        return handle_expr( node.sides[0] ).op_great( *this, handle_expr( node.sides[1] ) );

    case OperatorType::LESS_EQ:
        return handle_expr( node.sides[0] ).op_lesseq( *this, handle_expr( node.sides[1] ) );

    case OperatorType::GREAT_EQ:
        return handle_expr( node.sides[0] ).op_greateq( *this, handle_expr( node.sides[1] ) );

    case OperatorType::EQ:
        return handle_expr( node.sides[0] ).op_eq( *this, handle_expr( node.sides[1] ) );

    case OperatorType::NOT_EQ:
        return handle_expr( node.sides[0] ).op_neq( *this, handle_expr( node.sides[1] ) );

    case OperatorType::AND:
        return handle_expr( node.sides[0] ).op_and( *this, handle_expr( node.sides[1] ) );

    case OperatorType::OR:
        return handle_expr( node.sides[0] ).op_or( *this, handle_expr( node.sides[1] ) );

    case OperatorType::RANGE:
        return handle_expr( node.sides[0] ).op_range( *this, handle_expr( node.sides[1] ) );

    default:
        ENGINE_PANIC( "unknown operator node type: ", typeid(node).name() );
    }
}

dawn::ValueRef dawn::Engine::handle_ac_node( OperatorNod const& node )
{
    ValueRef left_val = handle_expr( node.sides[0] );

    if ( node.sides[1].type() != NodeType::IDENTIFIER )
        ENGINE_PANIC( "access must be an identifier" );

    Int right_id = node.sides[1].as<IdentifierNod>().id;
    if ( left_val.type() == ValueType::STRUCT )
        return handle_ac_struct_node( left_val, right_id );
    else
        return handle_ac_type_node( left_val, right_id );
}

dawn::ValueRef dawn::Engine::handle_as_node( AssignNod const& node )
{
    ValueRef left_val = handle_expr( node.sides[0] );

    switch ( node.type )
    {
    case AssignType::ASSIGN:
        left_val.set_value( handle_expr( node.sides[1] ).value() );
        return left_val;

    case AssignType::ADD:
        left_val.set_value( left_val.op_add( *this, handle_expr( node.sides[1] ) ).value() );
        return left_val;

    case AssignType::SUB:
        left_val.set_value( left_val.op_sub( *this, handle_expr( node.sides[1] ) ).value() );
        return left_val;

    case AssignType::MUL:
        left_val.set_value( left_val.op_mul( *this, handle_expr( node.sides[1] ) ).value() );
        return left_val;

    case AssignType::DIV:
        left_val.set_value( left_val.op_div( *this, handle_expr( node.sides[1] ) ).value() );
        return left_val;

    case AssignType::POW:
        left_val.set_value( left_val.op_pow( *this, handle_expr( node.sides[1] ) ).value() );
        return left_val;

    case AssignType::MOD:
        left_val.set_value( left_val.op_mod( *this, handle_expr( node.sides[1] ) ).value() );
        return left_val;

    default:
        ENGINE_PANIC( "unknown assign node type: ", typeid(node).name() );
    }
}

dawn::ValueRef dawn::Engine::handle_ac_struct_node( ValueRef const& left, Int right )
{
    auto& left_val = left.as<StructVal>();
    if ( !left_val.members.contains( right ) )
        ENGINE_PANIC( "struct [", IDSystem::get( left_val.parent->id ), "] doesn't have member [", IDSystem::get( right ), "]" );

    auto& result = left_val.members.at( right );
    if ( result.type() == ValueType::FUNCTION )
    {
        auto& func = result.as<Function>();
        func.self_vals.resize( 1 );
        func.self_vals[0] = left;
    }

    return result;
}

dawn::ValueRef dawn::Engine::handle_ac_type_node( ValueRef const& left, Int right )
{
    auto& members = type_members[(Int) left.type()];
    if ( !members.contains( right ) )
        ENGINE_PANIC( "type [", left.type(), "] doesn't have member [", IDSystem::get( right ), "]" );

    ValueRef result = members.at( right )(left);
    if ( result.type() == ValueType::FUNCTION )
    {
        auto& func = result.as<Function>();
        func.self_vals.resize( 1 );
        func.self_vals[0] = left;
    }

    return result;
}
