#include "engine.h"


void dawn::Engine::load_mod( Module const& module )
{
    for ( auto& entry : module.functions )
        functions[entry.name] = entry;

    for ( auto& entry : module.enums )
    {
        auto& enu = (enums[entry.name] = entry);
        for ( auto& [key, expr] : enu.keys_expr )
        {
            ValueRef key_val;
            handle_expr( expr.expr, key_val );
            enu.keys_value[key] = ValueRef{ key_val.value() };
        }
    }

    for ( auto& entry : module.structs )
        structs[entry.name] = entry;

    for ( auto& entry : module.variables )
        add_var( entry );
}

void dawn::Engine::bind_func( String const& name, Function::CppFunc cpp_func )
{
    Function func;
    func.name = name;
    func.body.emplace<Function::CppFunc>( std::move( cpp_func ) );
    functions[name] = func;
}

void dawn::Engine::call_func( String const& name, Array<Node> const& args, ValueRef& retval )
{
    FunctionNod node;
    node.name = name;
    node.args = args;
    handle_func_node( node, retval );
}

void dawn::Engine::add_var( Variable const& var )
{
    ValueRef var_val;
    handle_expr( var.expr, var_val );
    add_var( var.kind, var.name, var_val );
}

void dawn::Engine::add_var( VariableKind kind, StringRef const& name, ValueRef const& value )
{
    if ( kind == VariableKind::LET )
    {
        variables.push( name, ValueRef{ value.value() } );
    }
    else if ( kind == VariableKind::VAR )
    {
        variables.push( name, ValueRef{ value.value(), ValueKind::VAR } );
    }
    else
    {
        variables.push( name, value );
    }
}

dawn::ValueRef* dawn::Engine::get_var( String const& name )
{
    return variables.get( name );
}

void dawn::Engine::handle_func( Function const& func, Array<Node> const& args, ValueRef& retval )
{
    if ( func.body.index() == 0 )
    {
        if ( func.args.size() != args.size() )
            ENGINE_PANIC( "invalid argument count for function [", func.name, L"]" );

        for ( Int i = 0; i < (Int) args.size(); i++ )
        {
            ValueRef arg_val;
            handle_expr( args[i], arg_val );
            add_var( func.args[i].kind, func.args[i].name, arg_val );
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

        auto result = std::get<Function::CppFunc>( func.body )(arg_vals);
        retval = ValueRef{ result };
    }
}

void dawn::Engine::handle_scope( Scope const& scope, ValueRef& retval, Bool& didret, Bool* didbrk, Bool* didcon )
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

void dawn::Engine::handle_instr( Node const& node, ValueRef& retval, Int& push_count, Bool& didret, Bool* didbrk, Bool* didcon )
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

void dawn::Engine::handle_expr( Node const& node, ValueRef& value )
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

void dawn::Engine::handle_ref_node( RefNod const& node, ValueRef& value )
{
    value = node.value_ref;
}

void dawn::Engine::handle_cast_node( CastNod const& node, ValueRef& value )
{
    ValueRef cast_val;
    handle_expr( node.expr, cast_val );

    if ( node.type == tp_bool )
    {
        value = ValueRef{ Value{ cast_val.value().to_bool() } };
    }
    else if ( node.type == tp_int )
    {
        value = ValueRef{ Value{ cast_val.value().to_int() } };
    }
    else if ( node.type == tp_float )
    {
        value = ValueRef{ Value{ cast_val.value().to_float() } };
    }
    else if ( node.type == tp_char )
    {
        value = ValueRef{ Value{ cast_val.value().to_char() } };
    }
    else if ( node.type == tp_string )
    {
        value = ValueRef{ Value{ cast_val.value().to_string() } };
    }
    else
        ENGINE_PANIC( "Unknown cast type: ", node.type );
}

void dawn::Engine::handle_var_node( VariableNod const& node, Int& push_count )
{
    add_var( node.var );
    ++push_count;
}

void dawn::Engine::handle_id_node( IdentifierNod const& node, ValueRef& value )
{
    auto* ptr = variables.get( node.name );
    if ( !ptr )
        ENGINE_PANIC( L"variable [", node.name, L"] doesn't exist" );
    value = *ptr;
}

void dawn::Engine::handle_func_node( FunctionNod const& node, ValueRef& retval )
{
    auto it = functions.find( node.name );
    if ( it == functions.end() )
        ENGINE_PANIC( L"function [", node.name, L"] doesn't exist" );
    handle_func( it->second, node.args, retval );
}

void dawn::Engine::handle_return_node( ReturnNod const& node, ValueRef& retval, Bool& didret )
{
    handle_expr( node.expr, retval );
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

void dawn::Engine::handle_if_node( IfNod const& node, ValueRef& retval, Bool& didret, Bool* didbrk, Bool* didcon )
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

void dawn::Engine::handle_switch_node( SwitchNod const& node, ValueRef& retval, Bool& didret, Bool* didbrk, Bool* didcon )
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

void dawn::Engine::handle_loop_node( LoopNod const& node, ValueRef& retval, Bool& didret )
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

void dawn::Engine::handle_while_node( WhileNod const& node, ValueRef& retval, Bool& didret )
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

void dawn::Engine::handle_for_node( ForNod const& node, ValueRef& retval, Bool& didret )
{
    ValueRef loop_val;
    handle_expr( node.expr, loop_val );

    if ( loop_val.value().type() == ValueType::STRING )
    {
        auto& value_str = loop_val.value().as<String>();

        Bool didbrk = false, didcon = false;
        for ( Char c : value_str )
        {
            if ( didret || didbrk )
                break;

            if ( didcon )
                didcon = false;

            Variable arg = node.var;
            arg.expr = make_char_node( c );

            add_var( arg );

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

            add_var( node.var.kind, node.var.name, value );

            handle_scope( node.scope, retval, didret, &didbrk, &didcon );

            variables.pop();
        }
    }
    else if ( loop_val.value().type() == ValueType::RANGE )
    {
        auto& value_rng = loop_val.value().as<RangeVal>();

        Bool didbrk = false, didcon = false;
        for ( auto i = value_rng.start_incl; i < value_rng.end_excl; ++i )
        {
            if ( didret || didbrk )
                break;

            if ( didcon )
                didcon = false;

            Variable arg = node.var;
            arg.expr = make_int_node( i );

            add_var( arg );

            handle_scope( node.scope, retval, didret, &didbrk, &didcon );

            variables.pop();
        }
    }
    else
        ENGINE_PANIC( "Can't for loop [", loop_val.value().type(), "]" );
}

void dawn::Engine::handle_enum_node( EnumNod const& node, ValueRef& value )
{
    if ( !enums.contains( node.type ) )
        ENGINE_PANIC( "enum [", node.type, L"] doesn't exist" );

    EnumVal result{};
    result.parent = &enums.at( node.type );
    result.key = node.key;

    value = ValueRef{ result };
}

void dawn::Engine::handle_struct_node( StructNod const& node, ValueRef& value )
{
    if ( !structs.contains( node.type ) )
        ENGINE_PANIC( "struct [", node.type, L"] doesn't exist" );

    auto& struc = structs.at( node.type );

    StructVal result{};
    result.parent = &struc;

    for ( auto& struc_field : struc.fields )
    {
        auto& field = result.members[struc_field.name];
        if ( node.args.contains( struc_field.name ) )
            continue;

        ValueRef field_val;
        handle_expr( struc_field.expr, field_val );
        field = ValueRef{ field_val.value() };
    }

    for ( auto& [arg_name, arg_expr] : node.args )
    {
        ValueRef arg_val;
        handle_expr( arg_expr, arg_val );

        if ( !result.members.contains( arg_name ) )
            ENGINE_PANIC( "field [", arg_name, L"] doesn't exist in struct [", node.type, L"]" );

        result.members.at( arg_name ) = ValueRef{ arg_val.value() };
    }

    value = ValueRef{ result };
}

void dawn::Engine::handle_array_node( ArrayNod const& node, ValueRef& value )
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

void dawn::Engine::handle_un_node( UnaryNod const& node, ValueRef& value )
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

void dawn::Engine::handle_op_node( OperatorNod const& node, ValueRef& value )
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

void dawn::Engine::handle_ac_node( OperatorNod const& node, ValueRef& value )
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

void dawn::Engine::handle_as_node( AssignNod const& node, ValueRef& value )
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

void dawn::Engine::handle_ac_string_node( ValueRef const& left, Node const& right, ValueRef& value )
{
    auto& left_val = left.value().as<String>();

    if ( right.type() == NodeType::IDENTIFIER )
    {
        auto& id_node = right.as<IdentifierNod>();

        if ( id_node.name == L"length" )
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

void dawn::Engine::handle_ac_enum_node( ValueRef const& left, Node const& right, ValueRef& value )
{
    auto& left_val = left.value().as<EnumVal>();

    if ( right.type() == NodeType::IDENTIFIER )
    {
        auto& id_node = right.as<IdentifierNod>();

        if ( id_node.name == L"value" )
        {
            value = left_val.parent->keys_value.at( left_val.key );
            return;
        }

        ENGINE_PANIC( "Enum access [", id_node.name, "] doesn't exist" );
    }
    else
        ENGINE_PANIC( "Enum access must be an identifier" );
}

void dawn::Engine::handle_ac_struct_node( ValueRef const& left, Node const& right, ValueRef& value )
{
    auto& left_val = left.value().as<StructVal>();

    if ( right.type() == NodeType::IDENTIFIER )
    {
        auto& id_node = right.as<IdentifierNod>();

        if ( !left_val.members.contains( id_node.name ) )
            ENGINE_PANIC( "Member [", id_node.name, "] doesn't exist" );

        value = left_val.members.at( id_node.name );
        return;
    }
    else if ( right.type() == NodeType::FUNCTION )
    {
        auto& func_node = right.as<FunctionNod>();

        auto method_ptr = left_val.parent->get_method( func_node.name );
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

void dawn::Engine::handle_ac_array_node( ValueRef const& left, Node const& right, ValueRef& value )
{
    auto& left_val = left.value().as<ArrayVal>();

    if ( right.type() == NodeType::IDENTIFIER )
    {
        auto& id_node = right.as<IdentifierNod>();

        if ( id_node.name == L"length" )
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
