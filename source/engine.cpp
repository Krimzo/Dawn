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
    {
        ValueRef key_val;
        handle_expr( expr, key_val );
        enu.keys_value[key] = ValueRef{ key_val.value() };
    }
}

void dawn::Engine::load_struct( Struct const& entry )
{
    structs[entry.id] = entry;
}

void dawn::Engine::load_variable( Variable const& entry )
{
    ValueRef value;
    handle_expr( entry.expr.value(), value );
    add_obj( entry.kind, entry.id, value );
}

void dawn::Engine::bind_func( Int id, Function::CppFunc cpp_func )
{
    Function func;
    func.id = id;
    func.body.emplace<Function::CppFunc>( std::move( cpp_func ) );
    load_function( func );
}

void dawn::Engine::call_func( Int id, ValueRef const* args, Int arg_count, ValueRef& retval )
{
    ValueRef* val = stack.root().get( id );
    if ( !val )
        ENGINE_PANIC( "object [", IDSystem::get( id ), "] doesn't exist" );

    if ( val->type() != ValueType::FUNCTION )
        ENGINE_PANIC( "object [", IDSystem::get( id ), "] can't be called" );

    handle_func( val->as<Function>(), args, arg_count, retval );
}

void dawn::Engine::add_obj( VariableKind kind, Int id, ValueRef const& value )
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

dawn::ValueRef* dawn::Engine::get_obj( Int id )
{
    return stack.current().get( id );
}

void dawn::Engine::handle_func( Function const& func, ValueRef const* args, Int arg_count, ValueRef& retval )
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

        auto stack_helper = stack.push( func );

        for ( Int i = 0; i < arg_count; i++ )
            add_obj( func.args[i].kind, func.args[i].id, args[i] );

        Bool didret = false;
        handle_scope( *std::get_if<Scope>( &func.body ), retval, didret, nullptr, nullptr );
        if ( !didret )
            retval = ValueRef{ Value{} };
    }
    else
    {
        retval = (*std::get_if<Function::CppFunc>( &func.body ))(args, arg_count);
    }
}

void dawn::Engine::handle_scope( Scope const& scope, ValueRef& retval, Bool& didret, Bool* didbrk, Bool* didcon )
{
    for ( auto& instr : scope.instr )
    {
        if ( didret || (didbrk && *didbrk) || (didcon && *didcon) )
            break;

        handle_instr( instr, retval, didret, didbrk, didcon );
    }
}

void dawn::Engine::handle_instr( Node const& node, ValueRef& retval, Bool& didret, Bool* didbrk, Bool* didcon )
{
    switch ( node.type() )
    {
    case NodeType::SCOPE:
    {
        auto stack_helper = stack.push();
        handle_scope( node.as<Scope>(), retval, didret, didbrk, didcon );
    }
    break;

    case NodeType::VARIABLE:
        handle_var_node( node.as<VariableNod>() );
        break;

    case NodeType::RETURN:
        handle_return_node( node.as<ReturnNod>(), retval, didret );
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
        handle_try_node( node.as<TryNod>(), retval, didret, didbrk, didcon );
        break;

    case NodeType::IF:
        handle_if_node( node.as<IfNod>(), retval, didret, didbrk, didcon );
        break;

    case NodeType::SWITCH:
        handle_switch_node( node.as<SwitchNod>(), retval, didret, didbrk, didcon );
        break;

    case NodeType::LOOP:
        handle_loop_node( node.as<LoopNod>(), retval, didret );
        break;

    case NodeType::WHILE:
        handle_while_node( node.as<WhileNod>(), retval, didret );
        break;

    case NodeType::FOR:
        handle_for_node( node.as<ForNod>(), retval, didret );
        break;

    default:
        handle_expr( node, retval );
    }
}

void dawn::Engine::handle_expr( Node const& node, ValueRef& value )
{
    switch ( node.type() )
    {
    case NodeType::REF:
        handle_ref_node( node.as<RefNod>(), value );
        break;

    case NodeType::IDENTIFIER:
        handle_id_node( node.as<IdentifierNod>(), value );
        break;

    case NodeType::CALL:
        handle_call_node( node.as<CallNod>(), value );
        break;

    case NodeType::INDEX:
        handle_index_node( node.as<IndexNod>(), value );
        break;

    case NodeType::ENUM:
        handle_enum_node( node.as<EnumNod>(), value );
        break;

    case NodeType::STRUCT:
        handle_struct_node( node.as<StructNod>(), value );
        break;

    case NodeType::ARRAY:
        handle_array_node( node.as<ArrayNod>(), value );
        break;

    case NodeType::UNARY:
        handle_un_node( node.as<UnaryNod>(), value );
        break;

    case NodeType::OPERATOR:
        handle_op_node( node.as<OperatorNod>(), value );
        break;

    case NodeType::ASSIGN:
        handle_as_node( node.as<AssignNod>(), value );
        break;

    default:
        ENGINE_PANIC( "unknown expr node type: ", (Int) node.type() );
    }
}

void dawn::Engine::handle_ref_node( RefNod const& node, ValueRef& value )
{
    if ( node.value_ref.type() == ValueType::FUNCTION )
    {
        auto& func = node.value_ref.as<Function>();
        if ( func.is_lambda() )
            func.lambda_parent = stack.peek();
    }
    value = node.value_ref;
}

void dawn::Engine::handle_var_node( VariableNod const& node )
{
    ValueRef value;
    handle_expr( node.var.expr.value(), value );
    add_obj( node.var.kind, node.var.id, value );
}

void dawn::Engine::handle_id_node( IdentifierNod const& node, ValueRef& value )
{
    auto* ptr = get_obj( node.id );
    if ( !ptr )
        ENGINE_PANIC( "object [", IDSystem::get( node.id ), "] doesn't exist" );
    value = *ptr;
}

void dawn::Engine::handle_call_node( CallNod const& node, ValueRef& retval )
{
    ValueRef left_val;
    handle_expr( node.left_expr.value(), left_val );

    if ( left_val.type() != ValueType::FUNCTION )
        ENGINE_PANIC( "can't call [", left_val.type(), "]" );
    auto& func = left_val.as<Function>();

    Int arg_count = func.is_method() ? (1 + node.args.size()) : node.args.size();
    ValueRef* args_ptr = (ValueRef*) _alloca( arg_count * sizeof( ValueRef ) );
    for ( Int i = 0; i < arg_count; i++ )
        new (args_ptr + i) ValueRef();

    struct Destructor
    {
        ValueRef* args_ptr;
        Int arg_count;

        Destructor( ValueRef* args_ptr, Int arg_count )
            : args_ptr( args_ptr ), arg_count( arg_count )
        {
        }

        ~Destructor() noexcept
        {
            for ( Int i = 0; i < arg_count; i++ )
                args_ptr[i].~ValueRef();
        }
    };

    Destructor destructor{ args_ptr, arg_count };

    if ( func.is_method() )
    {
        args_ptr[0] = func.self_vals[0];
        for ( Int i = 0; i < (Int) node.args.size(); i++ )
            handle_expr( node.args[i], args_ptr[1 + i] );
    }
    else
    {
        for ( Int i = 0; i < (Int) node.args.size(); i++ )
            handle_expr( node.args[i], args_ptr[i] );
    }

    handle_func( func, args_ptr, arg_count, retval );
}

void dawn::Engine::handle_index_node( IndexNod const& node, ValueRef& retval )
{
    ValueRef left_val;
    handle_expr( node.left_expr.value(), left_val );

    ValueRef expr_val;
    handle_expr( node.expr.value(), expr_val );
    Int index = expr_val.to_int( *this );

    if ( left_val.type() == ValueType::STRING )
    {
        auto& val = left_val.as<String>();
        if ( index < 0 || index >= (Int) val.size() )
            ENGINE_PANIC( "string access [", index, "] out of bounds" );
        retval = ValueRef{ val[index] };
    }
    else if ( left_val.type() == ValueType::ARRAY )
    {
        auto& val = left_val.as<ArrayVal>();
        if ( index < 0 || index >= (Int) val.data.size() )
            ENGINE_PANIC( "array access [", index, "] out of bounds" );
        retval = val.data[index];
    }
    else
        ENGINE_PANIC( "can't index type [", left_val.type(), "]" );
}

void dawn::Engine::handle_return_node( ReturnNod const& node, ValueRef& retval, Bool& didret )
{
    handle_expr( node.expr.value(), retval );
    didret = true;
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
    ValueRef throw_val;
    handle_expr( node.expr.value(), throw_val );
    throw throw_val;
}

void dawn::Engine::handle_try_node( TryNod const& node, ValueRef& retval, Bool& didret, Bool* didbrk, Bool* didcon )
{
    try
    {
        auto stack_helper = stack.push();
        handle_scope( node.try_scope, retval, didret, didbrk, didcon );
    }
    catch ( ValueRef const& val )
    {
        auto stack_helper = stack.push();
        add_obj( VariableKind::REF, node.catch_id, val );
        handle_scope( node.catch_scope, retval, didret, didbrk, didcon );
    }
}

void dawn::Engine::handle_if_node( IfNod const& node, ValueRef& retval, Bool& didret, Bool* didbrk, Bool* didcon )
{
    ValueRef check_val;
    for ( auto& part : node.parts )
    {
        handle_expr( part.expr, check_val );
        if ( check_val.to_bool( *this ) )
        {
            auto stack_helper = stack.push();
            handle_scope( part.scope, retval, didret, didbrk, didcon );
            break;
        }
    }
}

void dawn::Engine::handle_switch_node( SwitchNod const& node, ValueRef& retval, Bool& didret, Bool* didbrk, Bool* didcon )
{
    ValueRef check_val;
    handle_expr( node.main_expr.value(), check_val );

    for ( auto& case_part : node.cases )
    {
        for ( auto& expr : case_part.exprs )
        {
            ValueRef case_val;
            handle_expr( expr, case_val );

            if ( check_val.op_eq( *this, case_val ).to_bool( *this ) )
            {
                auto stack_helper = stack.push();
                handle_scope( case_part.scope, retval, didret, didbrk, didcon );
                return;
            }
        }
    }

    if ( node.def_scope )
    {
        auto stack_helper = stack.push();
        handle_scope( *node.def_scope, retval, didret, didbrk, didcon );
    }
}

void dawn::Engine::handle_loop_node( LoopNod const& node, ValueRef& retval, Bool& didret )
{
    Bool didbrk = false, didcon = false;
    while ( true )
    {
        if ( didret || didbrk )
            break;
        didcon = false;

        auto stack_helper = stack.push();
        handle_scope( node.scope, retval, didret, &didbrk, &didcon );
    }
}

void dawn::Engine::handle_while_node( WhileNod const& node, ValueRef& retval, Bool& didret )
{
    Bool didbrk = false, didcon = false;
    while ( true )
    {
        if ( didret || didbrk )
            break;
        didcon = false;

        ValueRef check_val;
        handle_expr( node.expr.value(), check_val );

        if ( !check_val.to_bool( *this ) )
            break;

        auto stack_helper = stack.push();
        handle_scope( node.scope, retval, didret, &didbrk, &didcon );
    }
}

void dawn::Engine::handle_for_node( ForNod const& node, ValueRef& retval, Bool& didret )
{
    ValueRef loop_val;
    handle_expr( node.expr.value(), loop_val );

    if ( loop_val.type() == ValueType::RANGE )
    {
        auto& value_rng = loop_val.as<RangeVal>();

        Bool didbrk = false, didcon = false;
        for ( Int i = value_rng.start_incl; i < value_rng.end_excl; ++i )
        {
            if ( didret || didbrk )
                break;
            didcon = false;

            auto stack_helper = stack.push();
            add_obj( node.var.kind, node.var.id, ValueRef{ i } );
            handle_scope( node.scope, retval, didret, &didbrk, &didcon );
        }
    }
    else if ( loop_val.type() == ValueType::STRING )
    {
        auto& value_str = loop_val.as<String>();

        Bool didbrk = false, didcon = false;
        for ( Char c : value_str )
        {
            if ( didret || didbrk )
                break;
            didcon = false;

            auto stack_helper = stack.push();
            add_obj( node.var.kind, node.var.id, ValueRef{ c } );
            handle_scope( node.scope, retval, didret, &didbrk, &didcon );
        }
    }
    else if ( loop_val.type() == ValueType::ARRAY )
    {
        auto& value_arr = loop_val.as<ArrayVal>();

        Bool didbrk = false, didcon = false;
        for ( auto& value : value_arr.data )
        {
            if ( didret || didbrk )
                break;
            didcon = false;

            auto stack_helper = stack.push();
            add_obj( node.var.kind, node.var.id, value );
            handle_scope( node.scope, retval, didret, &didbrk, &didcon );
        }
    }
    else
        ENGINE_PANIC( "can't for loop [", loop_val.type(), "]" );
}

void dawn::Engine::handle_enum_node( EnumNod const& node, ValueRef& value )
{
    auto enum_it = enums.find( node.type_id );
    if ( enum_it == enums.end() )
        ENGINE_PANIC( "enum [", IDSystem::get( node.type_id ), "] doesn't exist" );

    if ( !enum_it->second.keys_value.contains( node.key_id ) )
        ENGINE_PANIC( "enum [", IDSystem::get( node.type_id ), "] doesn't have key [", IDSystem::get( node.key_id ), "]" );

    EnumVal result{};
    result.parent = &enum_it->second;
    result.key_id = node.key_id;

    value = ValueRef{ result };
}

void dawn::Engine::handle_struct_node( StructNod const& node, ValueRef& value )
{
    auto struct_it = structs.find( node.type_id );
    if ( struct_it == structs.end() )
        ENGINE_PANIC( "struct [", IDSystem::get( node.type_id ), "] doesn't exist" );

    value = ValueRef{ StructVal{} };
    auto& result = value.as<StructVal>();
    result.parent = &struct_it->second;

    for ( auto& field : struct_it->second.fields )
    {
        auto arg_it = std::find_if( node.args.begin(), node.args.end(), [&]( auto& arg ) { return arg.first == field.id; } );
        if ( arg_it != node.args.end() )
            continue;

        ValueRef field_val;
        handle_expr( field.expr.value(), field_val );
        result.members[field.id] = ValueRef{ field_val.value() };
    }

    for ( auto& [id, arg] : node.args )
    {
        auto field_it = std::find_if( struct_it->second.fields.begin(), struct_it->second.fields.end(), [&]( auto& field ) { return field.id == id; } );
        if ( field_it == struct_it->second.fields.end() )
            ENGINE_PANIC( "struct [", IDSystem::get( node.type_id ), "] doesn't contain member [", IDSystem::get( id ), "]" );

        ValueRef arg_val;
        handle_expr( arg, arg_val );
        result.members[id] = ValueRef{ arg_val.value() };
    }

    for ( auto& method : struct_it->second.methods )
        result.members[method.id] = ValueRef{ method };
}

void dawn::Engine::handle_array_node( ArrayNod const& node, ValueRef& value )
{
    ArrayVal result{};

    if ( node.init_type == ArrayNod::InitType::SIZE )
    {
        ValueRef size_val;
        handle_expr( node.SIZE_size_expr.value(), size_val );

        Int size = size_val.to_int( *this );
        if ( size < 0 )
            ENGINE_PANIC( "array size can't be negative" );

        ValueRef value_val;
        handle_expr( node.SIZE_value_expr.value(), value_val );

        result.data.resize( size );
        for ( auto& val : result.data )
            val = ValueRef{ value_val.value() };
    }
    else
    {
        result.data.reserve( node.LIST_list.size() );
        for ( auto& expr : node.LIST_list )
        {
            ValueRef entry_val;
            handle_expr( expr, entry_val );
            result.data.emplace_back( entry_val.value() );
        }
    }

    value = ValueRef{ result };
}

void dawn::Engine::handle_un_node( UnaryNod const& node, ValueRef& value )
{
    ValueRef right_val;
    handle_expr( node.right.value(), right_val );

    switch ( node.type )
    {
    case UnaryType::PLUS:
        value = ValueRef{ right_val.un_plus( *this ) };
        break;

    case UnaryType::MINUS:
        value = ValueRef{ right_val.un_minus( *this ) };
        break;

    case UnaryType::NOT:
        value = ValueRef{ right_val.un_not( *this ) };
        break;

    default:
        ENGINE_PANIC( "unknown unary node type: ", typeid(node).name() );
    }
}

void dawn::Engine::handle_op_node( OperatorNod const& node, ValueRef& value )
{
    if ( OperatorType::ACCESS == node.type )
        return handle_ac_node( node, value );

    ValueRef left_val;
    handle_expr( node.sides[0], left_val );

    ValueRef right_val;
    handle_expr( node.sides[1], right_val );

    switch ( node.type )
    {
    case OperatorType::POW:
        value = left_val.op_pow( *this, right_val );
        break;

    case OperatorType::MOD:
        value = left_val.op_mod( *this, right_val );
        break;

    case OperatorType::MUL:
        value = left_val.op_mul( *this, right_val );
        break;

    case OperatorType::DIV:
        value = left_val.op_div( *this, right_val );
        break;

    case OperatorType::ADD:
        value = left_val.op_add( *this, right_val );
        break;

    case OperatorType::SUB:
        value = left_val.op_sub( *this, right_val );
        break;

    case OperatorType::COMPARE:
        value = left_val.op_cmpr( *this, right_val );
        break;

    case OperatorType::LESS:
        value = left_val.op_less( *this, right_val );
        break;

    case OperatorType::GREAT:
        value = left_val.op_great( *this, right_val );
        break;

    case OperatorType::LESS_EQ:
        value = left_val.op_lesseq( *this, right_val );
        break;

    case OperatorType::GREAT_EQ:
        value = left_val.op_greateq( *this, right_val );
        break;

    case OperatorType::EQ:
        value = left_val.op_eq( *this, right_val );
        break;

    case OperatorType::NOT_EQ:
        value = left_val.op_neq( *this, right_val );
        break;

    case OperatorType::AND:
        value = left_val.op_and( *this, right_val );
        break;

    case OperatorType::OR:
        value = left_val.op_or( *this, right_val );
        break;

    case OperatorType::RANGE:
        value = left_val.op_range( *this, right_val );
        break;

    default:
        ENGINE_PANIC( "unknown operator node type: ", typeid(node).name() );
    }
}

void dawn::Engine::handle_ac_node( OperatorNod const& node, ValueRef& value )
{
    ValueRef left_val;
    handle_expr( node.sides[0], left_val );

    if ( node.sides[1].type() != NodeType::IDENTIFIER )
        ENGINE_PANIC( "access must be an identifier" );

    Int right_id = node.sides[1].as<IdentifierNod>().id;
    if ( left_val.type() == ValueType::STRUCT )
        return handle_ac_struct_node( left_val, right_id, value );
    else
        return handle_ac_type_node( left_val, right_id, value );
}

void dawn::Engine::handle_as_node( AssignNod const& node, ValueRef& value )
{
    ValueRef left_val;
    handle_expr( node.sides[0], left_val );

    ValueRef right_val;
    handle_expr( node.sides[1], right_val );

    switch ( node.type )
    {
    case AssignType::ASSIGN:
        left_val.set_value( right_val.value() );
        break;

    case AssignType::ADD:
        left_val.set_value( left_val.op_add( *this, right_val ).value() );
        break;

    case AssignType::SUB:
        left_val.set_value( left_val.op_sub( *this, right_val ).value() );
        break;

    case AssignType::MUL:
        left_val.set_value( left_val.op_mul( *this, right_val ).value() );
        break;

    case AssignType::DIV:
        left_val.set_value( left_val.op_div( *this, right_val ).value() );
        break;

    case AssignType::POW:
        left_val.set_value( left_val.op_pow( *this, right_val ).value() );
        break;

    case AssignType::MOD:
        left_val.set_value( left_val.op_mod( *this, right_val ).value() );
        break;

    default:
        ENGINE_PANIC( "unknown assign node type: ", typeid(node).name() );
    }

    value = left_val;
}

void dawn::Engine::handle_ac_struct_node( ValueRef const& left, Int right, ValueRef& value )
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

    value = result;
}

void dawn::Engine::handle_ac_type_node( ValueRef const& left, Int right, ValueRef& value )
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

    value = result;
}
