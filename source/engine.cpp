#include "engine.h"


void dawn::Engine::load_mod( Module& module )
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

void dawn::Engine::load_function( Function& entry )
{
    functions[entry.name.get( id_system )] = entry;
}

void dawn::Engine::load_enum( Enum& entry )
{
    auto& enu = (enums[entry.name.get( id_system )] = entry);
    for ( auto& [key, expr] : enu.keys_expr )
    {
        ValueRef key_val;
        handle_expr( expr.expr, key_val );
        enu.keys_value[id_system.get( key )] = ValueRef{ key_val.value() };
    }
}

void dawn::Engine::load_struct( Struct& entry )
{
    structs[entry.name.get( id_system )] = entry;
}

void dawn::Engine::load_variable( Variable& entry )
{
    add_var( entry );
}

void dawn::Engine::bind_func( StringRef const& name, Function::CppFunc cpp_func )
{
    Function func;
    func.name = String( name );
    func.body.emplace<Function::CppFunc>( std::move( cpp_func ) );
    functions[func.name.get( id_system )] = func;
}

void dawn::Engine::call_func( Int id, Array<Node>& args, ValueRef& retval )
{
    auto it = functions.find( id );
    if ( it == functions.end() )
    {
        String const* ptr = id_system.get( id );
        String name = ptr ? *ptr : String();
        ENGINE_PANIC( "function [", name, "] doesn't exist" );
    }
    handle_func( it->second, args, retval );
}

void dawn::Engine::add_var( Variable& var )
{
    ValueRef var_val;
    handle_expr( var.expr, var_val );
    add_var( var.kind, var.name.get( id_system ), var_val );
}

void dawn::Engine::add_var( VariableKind kind, Int id, ValueRef const& value )
{
    if ( kind == VariableKind::LET )
    {
        variables.push( id, ValueRef{ value.value() } );
    }
    else if ( kind == VariableKind::VAR )
    {
        variables.push( id, ValueRef{ value.value(), ValueKind::VAR } );
    }
    else
    {
        variables.push( id, value );
    }
}

dawn::ValueRef* dawn::Engine::get_var( Int id )
{
    return variables.get( id );
}

void dawn::Engine::handle_func( Function& func, Array<Node>& args, ValueRef& retval )
{
    if ( func.body.index() == 0 )
    {
        if ( func.args.size() != args.size() )
            ENGINE_PANIC( "invalid argument count for function [", func.name, "]" );

        for ( Int i = 0; i < (Int) args.size(); i++ )
        {
            ValueRef arg_val;
            handle_expr( args[i], arg_val );
            add_var( func.args[i].kind, func.args[i].name.get( id_system ), arg_val );
        }

        Bool didret = false;
        handle_scope( std::get<Scope>( func.body ), retval, didret, nullptr, nullptr );

        variables.pop( (Int) args.size() );
    }
    else
    {
        Array<ValueRef> arg_vals;
        arg_vals.reserve( args.size() );

        for ( auto& arg : args )
            handle_expr( arg, arg_vals.emplace_back() );

        retval = std::get<Function::CppFunc>( func.body )(arg_vals);
    }
}

void dawn::Engine::handle_scope( Scope& scope, ValueRef& retval, Bool& didret, Bool* didbrk, Bool* didcon )
{
    Int push_count = 0;

    for ( auto& instr : scope.instr )
    {
        if ( didret || (didbrk && *didbrk) || (didcon && *didcon) )
            break;

        handle_instr( instr, retval, push_count, didret, didbrk, didcon );
    }

    variables.pop( push_count );
}

void dawn::Engine::handle_instr( Node& node, ValueRef& retval, Int& push_count, Bool& didret, Bool* didbrk, Bool* didcon )
{
    switch ( node.type() )
    {
    case NodeType::SCOPE:
        handle_scope( node.as<Scope>(), retval, didret, didbrk, didcon );
        break;

    case NodeType::VARIABLE:
        handle_var_node( node.as<VariableNod>(), push_count );
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

void dawn::Engine::handle_expr( Node& node, ValueRef& value )
{
    switch ( node.type() )
    {
    case NodeType::REF:
        handle_ref_node( node.as<RefNod>(), value );
        break;

    case NodeType::CAST:
        handle_cast_node( node.as<CastNod>(), value );
        break;

    case NodeType::IDENTIFIER:
        handle_id_node( node.as<IdentifierNod>(), value );
        break;

    case NodeType::FUNCTION:
        handle_func_node( node.as<FunctionNod>(), value );
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
        ENGINE_PANIC( "Unknown expr node type: ", typeid(node).name() );
    }
}

void dawn::Engine::handle_ref_node( RefNod& node, ValueRef& value )
{
    value = node.value_ref;
}

void dawn::Engine::handle_cast_node( CastNod& node, ValueRef& value )
{
    ValueRef cast_val;
    handle_expr( node.expr, cast_val );

    if ( node.type.get( id_system ) == pre_names._bool.get( id_system ) )
    {
        value = ValueRef{ Value{ cast_val.value().to_bool() } };
    }
    else if ( node.type.get( id_system ) == pre_names._int.get( id_system ) )
    {
        value = ValueRef{ Value{ cast_val.value().to_int() } };
    }
    else if ( node.type.get( id_system ) == pre_names._float.get( id_system ) )
    {
        value = ValueRef{ Value{ cast_val.value().to_float() } };
    }
    else if ( node.type.get( id_system ) == pre_names._char.get( id_system ) )
    {
        value = ValueRef{ Value{ cast_val.value().to_char() } };
    }
    else if ( node.type.get( id_system ) == pre_names._string.get( id_system ) )
    {
        value = ValueRef{ Value{ cast_val.value().to_string() } };
    }
    else
        ENGINE_PANIC( "Unknown cast type: ", node.type );
}

void dawn::Engine::handle_var_node( VariableNod& node, Int& push_count )
{
    add_var( node.var );
    ++push_count;
}

void dawn::Engine::handle_id_node( IdentifierNod& node, ValueRef& value )
{
    auto* ptr = variables.get( node.name.get( id_system ) );
    if ( !ptr )
        ENGINE_PANIC( "variable [", node.name, "] doesn't exist" );
    value = *ptr;
}

void dawn::Engine::handle_func_node( FunctionNod& node, ValueRef& retval )
{
    auto it = functions.find( node.name.get( id_system ) );
    if ( it == functions.end() )
        ENGINE_PANIC( "function [", node.name, "] doesn't exist" );
    handle_func( it->second, node.args, retval );
}

void dawn::Engine::handle_return_node( ReturnNod& node, ValueRef& retval, Bool& didret )
{
    handle_expr( node.expr, retval );
    didret = true;
}

void dawn::Engine::handle_break_node( BreakNod& node, Bool* didbrk )
{
    if ( !didbrk )
        ENGINE_PANIC( "break statement outside of loop" );
    *didbrk = true;
}

void dawn::Engine::handle_continue_node( ContinueNod& node, Bool* didcon )
{
    if ( !didcon )
        ENGINE_PANIC( "continue statement outside of loop" );
    *didcon = true;
}

void dawn::Engine::handle_if_node( IfNod& node, ValueRef& retval, Bool& didret, Bool* didbrk, Bool* didcon )
{
    ValueRef check_val;
    handle_expr( node.if_part.expr, check_val );

    if ( check_val.value().to_bool() )
    {
        handle_scope( node.if_part.scope, retval, didret, didbrk, didcon );
        return;
    }

    for ( auto& elif_part : node.elif_parts )
    {
        handle_expr( elif_part.expr, check_val );
        if ( check_val.value().to_bool() )
        {
            handle_scope( elif_part.scope, retval, didret, didbrk, didcon );
            return;
        }
    }

    if ( node.else_part )
        handle_scope( node.else_part->scope, retval, didret, didbrk, didcon );
}

void dawn::Engine::handle_switch_node( SwitchNod& node, ValueRef& retval, Bool& didret, Bool* didbrk, Bool* didcon )
{
    ValueRef check_val;
    handle_expr( node.main_expr, check_val );

    for ( auto& case_part : node.cases )
    {
        for ( auto& expr : case_part.exprs )
        {
            ValueRef case_val;
            handle_expr( expr, case_val );

            if ( (check_val.value() == case_val.value()).to_bool() )
            {
                handle_scope( case_part.scope, retval, didret, didbrk, didcon );
                return;
            }
        }
    }

    if ( node.def_scope )
        handle_scope( *node.def_scope, retval, didret, didbrk, didcon );
}

void dawn::Engine::handle_loop_node( LoopNod& node, ValueRef& retval, Bool& didret )
{
    Bool didbrk = false, didcon = false;
    while ( true )
    {
        if ( didret || didbrk )
            break;

        if ( didcon )
            didcon = false;

        handle_scope( node.scope, retval, didret, &didbrk, &didcon );
    }
}

void dawn::Engine::handle_while_node( WhileNod& node, ValueRef& retval, Bool& didret )
{
    Bool didbrk = false, didcon = false;
    while ( true )
    {
        ValueRef check_val;
        handle_expr( node.expr, check_val );

        if ( !check_val.value().to_bool() )
            break;

        if ( didret || didbrk )
            break;

        if ( didcon )
            didcon = false;

        handle_scope( node.scope, retval, didret, &didbrk, &didcon );
    }
}

void dawn::Engine::handle_for_node( ForNod& node, ValueRef& retval, Bool& didret )
{
    ValueRef loop_val;
    handle_expr( node.expr, loop_val );

    if ( loop_val.value().type() == ValueType::RANGE )
    {
        auto& value_rng = loop_val.value().as<RangeVal>();

        Bool didbrk = false, didcon = false;
        for ( Int i = value_rng.start_incl; i < value_rng.end_excl; ++i )
        {
            if ( didret || didbrk )
                break;

            if ( didcon )
                didcon = false;

            add_var( node.var.kind, node.var.name.get( id_system ), Value{ i } );
            handle_scope( node.scope, retval, didret, &didbrk, &didcon );
            variables.pop();
        }
    }
    else if ( loop_val.value().type() == ValueType::STRING )
    {
        auto& value_str = loop_val.value().as<String>();

        Bool didbrk = false, didcon = false;
        for ( Char c : value_str )
        {
            if ( didret || didbrk )
                break;

            if ( didcon )
                didcon = false;

            add_var( node.var.kind, node.var.name.get( id_system ), Value{ c } );
            handle_scope( node.scope, retval, didret, &didbrk, &didcon );
            variables.pop();
        }
    }
    else if ( loop_val.value().type() == ValueType::ARRAY )
    {
        auto& value_arr = loop_val.value().as<ArrayVal>();

        Bool didbrk = false, didcon = false;
        for ( auto& value : value_arr.data )
        {
            if ( didret || didbrk )
                break;

            if ( didcon )
                didcon = false;

            add_var( node.var.kind, node.var.name.get( id_system ), value );
            handle_scope( node.scope, retval, didret, &didbrk, &didcon );
            variables.pop();
        }
    }
    else
        ENGINE_PANIC( "Can't for loop [", loop_val.value().type(), "]" );
}

void dawn::Engine::handle_enum_node( EnumNod& node, ValueRef& value )
{
    auto enum_it = enums.find( node.type.get( id_system ) );
    if ( enum_it == enums.end() )
        ENGINE_PANIC( "enum [", node.type, "] doesn't exist" );

    if ( !enum_it->second.keys_value.contains( node.key.get( id_system ) ) )
        ENGINE_PANIC( "enum [", node.type, "] doesn't have key [", node.key, "]" );

    EnumVal result{};
    result.parent = &enum_it->second;
    result.key = node.key;

    value = ValueRef{ result };
}

void dawn::Engine::handle_struct_node( StructNod& node, ValueRef& value )
{
    auto struct_it = structs.find( node.type.get( id_system ) );
    if ( struct_it == structs.end() )
        ENGINE_PANIC( "struct [", node.type, "] doesn't exist" );

    StructVal result{};
    result.parent = &struct_it->second;

    for ( auto& field : struct_it->second.fields )
    {
        auto arg_it = std::find_if( node.args.begin(), node.args.end(), [&]( auto& arg ) { return arg.first.get( id_system ) == field.name.get( id_system ); } );
        if ( arg_it != node.args.end() )
            continue;

        ValueRef field_val;
        handle_expr( field.expr, field_val );
        result.members[field.name.get( id_system )] = ValueRef{ field_val.value() };
    }

    for ( auto& [id, arg] : node.args )
    {
        auto field_it = std::find_if( struct_it->second.fields.begin(), struct_it->second.fields.end(), [&]( auto& field ) { return field.name.get( id_system ) == id.get( id_system ); } );
        if ( field_it == struct_it->second.fields.end() )
            ENGINE_PANIC( "struct [", node.type, "] doesn't contain member [", id, "]" );

        ValueRef arg_val;
        handle_expr( arg, arg_val );
        result.members[id.get( id_system )] = ValueRef{ arg_val.value() };
    }

    value = ValueRef{ result };
}

void dawn::Engine::handle_array_node( ArrayNod& node, ValueRef& value )
{
    ArrayVal result{};

    if ( node.init_type == ArrayNod::InitType::SIZE )
    {
        ValueRef size_val;
        handle_expr( node.SIZE_size_expr, size_val );

        Int size = size_val.value().to_int();
        if ( size < 0 )
            ENGINE_PANIC( "Array size cannot be negative" );

        ValueRef value_val;
        handle_expr( node.SIZE_value_expr, value_val );

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

void dawn::Engine::handle_un_node( UnaryNod& node, ValueRef& value )
{
    ValueRef right_val;
    handle_expr( node.right, right_val );

    switch ( node.type )
    {
    case UnaryType::PLUS:
        value = ValueRef{ +right_val.value() };
        break;

    case UnaryType::MINUS:
        value = ValueRef{ -right_val.value() };
        break;

    case UnaryType::NOT:
        value = ValueRef{ !right_val.value() };
        break;

    case UnaryType::RANGE:
        value = ValueRef{ ~right_val.value() };
        break;

    default:
        ENGINE_PANIC( "Unknown unary node type: ", typeid(node).name() );
    }
}

void dawn::Engine::handle_op_node( OperatorNod& node, ValueRef& value )
{
    if ( OperatorType::ACCESS == node.type )
        return handle_ac_node( node, value );

    ValueRef left_val;
    handle_expr( node.left, left_val );

    ValueRef right_val;
    handle_expr( node.right, right_val );

    switch ( node.type )
    {
    case OperatorType::ADD:
        value = ValueRef{ left_val.value() + right_val.value() };
        break;

    case OperatorType::SUB:
        value = ValueRef{ left_val.value() - right_val.value() };
        break;

    case OperatorType::MUL:
        value = ValueRef{ left_val.value() * right_val.value() };
        break;

    case OperatorType::DIV:
        value = ValueRef{ left_val.value() / right_val.value() };
        break;

    case OperatorType::POW:
        value = ValueRef{ left_val.value() ^ right_val.value() };
        break;

    case OperatorType::MOD:
        value = ValueRef{ left_val.value() % right_val.value() };
        break;

    case OperatorType::AND:
        value = ValueRef{ left_val.value() && right_val.value() };
        break;

    case OperatorType::OR:
        value = ValueRef{ left_val.value() || right_val.value() };
        break;

    case OperatorType::EQ:
        value = ValueRef{ left_val.value() == right_val.value() };
        break;

    case OperatorType::NOT_EQ:
        value = ValueRef{ left_val.value() != right_val.value() };
        break;

    case OperatorType::LESS:
        value = ValueRef{ left_val.value() < right_val.value() };
        break;

    case OperatorType::GREAT:
        value = ValueRef{ left_val.value() > right_val.value() };
        break;

    case OperatorType::LESS_EQ:
        value = ValueRef{ left_val.value() <= right_val.value() };
        break;

    case OperatorType::GREAT_EQ:
        value = ValueRef{ left_val.value() >= right_val.value() };
        break;

    case OperatorType::RANGE:
        value = ValueRef{ left_val.value() >> right_val.value() };
        break;

    default:
        ENGINE_PANIC( "Unknown operator node type: ", typeid(node).name() );
    }
}

void dawn::Engine::handle_ac_node( OperatorNod& node, ValueRef& value )
{
    ValueRef left_val;
    handle_expr( node.left, left_val );

    if ( left_val.value().type() == ValueType::STRING )
        return handle_ac_string_node( left_val, node.right, value );

    if ( left_val.value().type() == ValueType::ENUM )
        return handle_ac_enum_node( left_val, node.right, value );

    if ( left_val.value().type() == ValueType::STRUCT )
        return handle_ac_struct_node( left_val, node.right, value );

    if ( left_val.value().type() == ValueType::ARRAY )
        return handle_ac_array_node( left_val, node.right, value );

    ENGINE_PANIC( "Can't access member of [", left_val.value().type(), "]" );
}

void dawn::Engine::handle_as_node( AssignNod& node, ValueRef& value )
{
    ValueRef left_val;
    handle_expr( node.left, left_val );

    ValueRef right_val;
    handle_expr( node.right, right_val );

    switch ( node.type )
    {
    case AssignType::ASSIGN:
        left_val.set_value( right_val.value() );
        break;

    case AssignType::ADD:
        left_val.set_value( left_val.value() + right_val.value() );
        break;

    case AssignType::SUB:
        left_val.set_value( left_val.value() - right_val.value() );
        break;

    case AssignType::MUL:
        left_val.set_value( left_val.value() * right_val.value() );
        break;

    case AssignType::DIV:
        left_val.set_value( left_val.value() / right_val.value() );
        break;

    case AssignType::POW:
        left_val.set_value( left_val.value() ^ right_val.value() );
        break;

    case AssignType::MOD:
        left_val.set_value( left_val.value() % right_val.value() );
        break;

    default:
        ENGINE_PANIC( "Unknown assign node type: ", typeid(node).name() );
    }

    value = left_val;
}

void dawn::Engine::handle_ac_string_node( ValueRef const& left, Node& right, ValueRef& value )
{
    auto& left_val = left.value().as<String>();

    if ( right.type() == NodeType::IDENTIFIER )
    {
        auto& id_node = right.as<IdentifierNod>();

        if ( id_node.name.get( id_system ) == pre_names._length.get( id_system ) )
        {
            value = ValueRef{ Value{ (Int) left_val.size() } };
            return;
        }
    }

    ValueRef right_val;
    handle_expr( right, right_val );

    Int index = right_val.value().to_int();
    if ( index < 0 || index >= (Int) left_val.size() )
        ENGINE_PANIC( "String access [", index, "] out of bounds" );

    value = ValueRef{ Value{ left_val[index] } };
}

void dawn::Engine::handle_ac_enum_node( ValueRef const& left, Node& right, ValueRef& value )
{
    auto& left_val = const_cast<EnumVal&>(left.value().as<EnumVal>());

    if ( right.type() == NodeType::IDENTIFIER )
    {
        auto& id_node = right.as<IdentifierNod>();

        if ( id_node.name.get( id_system ) == pre_names._value.get( id_system ) )
        {
            value = left_val.parent->keys_value.at( left_val.key.get( id_system ) );
            return;
        }

        ENGINE_PANIC( "Enum access [", id_node.name, "] doesn't exist" );
    }
    else
        ENGINE_PANIC( "Enum access must be an identifier" );
}

void dawn::Engine::handle_ac_struct_node( ValueRef const& left, Node& right, ValueRef& value )
{
    auto& left_val = const_cast<StructVal&>(left.value().as<StructVal>());

    if ( right.type() == NodeType::IDENTIFIER )
    {
        auto& id_node = right.as<IdentifierNod>();

        if ( !left_val.members.contains( id_node.name.get( id_system ) ) )
            ENGINE_PANIC( "Member [", id_node.name, "] doesn't exist" );

        value = left_val.members.at( id_node.name.get( id_system ) );
        return;
    }
    else if ( right.type() == NodeType::FUNCTION )
    {
        auto& func_node = right.as<FunctionNod>();

        auto method_ptr = left_val.parent->get_method( id_system, func_node.name.get( id_system ) );
        if ( !method_ptr )
            ENGINE_PANIC( "Method [", func_node.name, "] doesn't exist" );

        Array<Node> args( 1 );
        args.front().store<RefNod>().value_ref = left;
        args.insert( args.end(), func_node.args.begin(), func_node.args.end() );

        handle_func( *method_ptr, args, value );
        return;
    }
    else
        ENGINE_PANIC( "Struct access must be an identifier or function call" );
}

void dawn::Engine::handle_ac_array_node( ValueRef const& left, Node& right, ValueRef& value )
{
    auto& left_val = left.value().as<ArrayVal>();

    if ( right.type() == NodeType::IDENTIFIER )
    {
        auto& id_node = right.as<IdentifierNod>();

        if ( id_node.name.get( id_system ) == pre_names._length.get( id_system ) )
        {
            value = ValueRef{ Value{ (Int) left_val.data.size() } };
            return;
        }

        ENGINE_PANIC( "Array access [", id_node.name, "] doesn't exist" );
    }

    ValueRef right_val;
    handle_expr( right, right_val );

    Int index = right_val.value().to_int();
    if ( index < 0 || index >= (Int) left_val.data.size() )
        ENGINE_PANIC( "Array access [", index, "] out of bounds" );

    value = left_val.data[index];
}
