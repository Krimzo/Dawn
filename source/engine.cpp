#include "engine.h"


void dawn::Engine::load_mod( Module const& module )
{
    for ( auto& entry : module.functions )
        functions[entry.name] = entry;

    for ( auto& entry : module.structs )
        structs[entry.name] = entry;

    for ( auto& entry : module.enums )
    {
        auto& enu = (enums[entry.name] = entry);
        for ( auto& [key, expr] : enu.keys_expr )
        {
            ValueBox key_val;
            handle_expr( expr.expr, key_val );
            enu.keys_value[key] = ValueBox{ key_val.value() };
        }
    }

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

void dawn::Engine::call_func( String const& name, Array<Ref<Node>> const& args, ValueBox& retval )
{
    FunctionNode node;
    node.name = name;
    node.args = args;
    handle_func_node( node, retval );
}

void dawn::Engine::add_var( Variable const& var )
{
    ValueBox var_val;
    handle_expr( var.expr, var_val );
    add_var( var, var_val );
}

void dawn::Engine::add_var( Variable const& var, ValueBox const& value )
{
    if ( var.kind == VariableKind::LET )
    {
        variables.push( var.name, ValueBox{ value.value() } );
    }
    else if ( var.kind == VariableKind::VAR )
    {
        variables.push( var.name, ValueBox{ value.value(), ValueKind::VAR } );
    }
    else
    {
        variables.push( var.name, value );
    }
}

dawn::ValueBox* dawn::Engine::get_var( String const& name )
{
    return variables.get( name );
}

void dawn::Engine::handle_func( Function const& func, Array<Ref<Node>> const& args, ValueBox& retval )
{
    if ( func.body.index() == 0 )
    {
        if ( func.args.size() != args.size() )
            ENGINE_PANIC( "invalid argument count for function [", func.name, L"]" );

        for ( Int i = 0; i < (Int) args.size(); i++ )
        {
            Variable arg;
            arg.name = func.args[i].name;
            arg.kind = func.args[i].kind;
            arg.expr = args[i];
            add_var( arg );
        }

        Bool didret = false;
        handle_scope( std::get<Scope>( func.body ), retval, didret, nullptr, nullptr );

        variables.pop( (Int) args.size() );
    }
    else
    {
        Array<ValueBox> arg_vals;
        arg_vals.reserve( args.size() );

        for ( auto& arg : args )
            handle_expr( arg, arg_vals.emplace_back() );

        auto result = std::get<Function::CppFunc>( func.body )(arg_vals);
        retval = ValueBox{ result };
    }
}

void dawn::Engine::handle_scope( Scope const& scope, ValueBox& retval, Bool& didret, Bool* didbrk, Bool* didcon )
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

void dawn::Engine::handle_instr( Ref<Node> const& node, ValueBox& retval, Int& push_count, Bool& didret, Bool* didbrk, Bool* didcon )
{
    if ( auto scp = dynamic_cast<Scope*>(node.get()) )
        handle_scope( *scp, retval, didret, didbrk, didcon );

    else if ( auto nd = dynamic_cast<VariableNode*>(node.get()) )
        handle_var_node( *nd, push_count );

    else if ( auto nd = dynamic_cast<ReturnNode*>(node.get()) )
        handle_return_node( *nd, retval, didret );

    else if ( auto nd = dynamic_cast<BreakNode*>(node.get()) )
        handle_break_node( *nd, didbrk );

    else if ( auto nd = dynamic_cast<ContinueNode*>(node.get()) )
        handle_continue_node( *nd, didcon );

    else if ( auto nd = dynamic_cast<IfNode*>(node.get()) )
        handle_if_node( *nd, retval, didret, didbrk, didcon );

    else if ( auto nd = dynamic_cast<SwitchNode*>(node.get()) )
        handle_switch_node( *nd, retval, didret, didbrk, didcon );

    else if ( auto nd = dynamic_cast<LoopNode*>(node.get()) )
        handle_loop_node( *nd, retval, didret );

    else if ( auto nd = dynamic_cast<WhileNode*>(node.get()) )
        handle_while_node( *nd, retval, didret );

    else if ( auto nd = dynamic_cast<ForNode*>(node.get()) )
        handle_for_node( *nd, retval, didret );

    else
        handle_expr( node, retval );
}

void dawn::Engine::handle_expr( Ref<Node> const& node, ValueBox& value )
{
    if ( auto nd = dynamic_cast<ValueNode*>(node.get()) )
        return handle_val_node( *nd, value );

    if ( auto nd = dynamic_cast<BoxNode*>(node.get()) )
        return handle_box_node( *nd, value );

    if ( auto nd = dynamic_cast<CastNode*>(node.get()) )
        return handle_cast_node( *nd, value );

    if ( auto nd = dynamic_cast<IdentifierNode*>(node.get()) )
        return handle_id_node( *nd, value );

    if ( auto nd = dynamic_cast<FunctionNode*>(node.get()) )
        return handle_func_node( *nd, value );

    if ( auto nd = dynamic_cast<EnumNode*>(node.get()) )
        return handle_enum_node( *nd, value );

    if ( auto nd = dynamic_cast<StructNode*>(node.get()) )
        return handle_struct_node( *nd, value );

    if ( auto nd = dynamic_cast<ArrayNode*>(node.get()) )
        return handle_array_node( *nd, value );

    if ( auto nd = dynamic_cast<UnaryNode*>(node.get()) )
        return handle_un_node( *nd, value );

    if ( auto nd = dynamic_cast<OperatorNode*>(node.get()) )
        return handle_op_node( *nd, value );

    if ( auto nd = dynamic_cast<AssignNode*>(node.get()) )
        return handle_as_node( *nd, value );

    ENGINE_PANIC( "Unknown expr node type: ", typeid(*node).name() );
}

void dawn::Engine::handle_val_node( ValueNode const& node, ValueBox& value )
{
    value = ValueBox{ node.value };
}

void dawn::Engine::handle_box_node( BoxNode const& node, ValueBox& value )
{
    value = node.box;
}

void dawn::Engine::handle_cast_node( CastNode const& node, ValueBox& value )
{
    ValueBox cast_val;
    handle_expr( node.expr, cast_val );

    if ( node.type == tp_bool )
    {
        value = ValueBox{ Value{ cast_val.value().to_bool() } };
    }
    else if ( node.type == tp_int )
    {
        value = ValueBox{ Value{ cast_val.value().to_int() } };
    }
    else if ( node.type == tp_float )
    {
        value = ValueBox{ Value{ cast_val.value().to_float() } };
    }
    else if ( node.type == tp_char )
    {
        value = ValueBox{ Value{ cast_val.value().to_char() } };
    }
    else if ( node.type == tp_string )
    {
        value = ValueBox{ Value{ cast_val.value().to_string() } };
    }
    else
        ENGINE_PANIC( "Unknown cast type: ", node.type );
}

void dawn::Engine::handle_var_node( VariableNode const& node, Int& push_count )
{
    add_var( node.var );
    ++push_count;
}

void dawn::Engine::handle_id_node( IdentifierNode const& node, ValueBox& value )
{
    auto* ptr = variables.get( node.name );
    if ( !ptr )
        ENGINE_PANIC( L"variable [", node.name, L"] doesn't exist" );
    value = *ptr;
}

void dawn::Engine::handle_func_node( FunctionNode const& node, ValueBox& retval )
{
    if ( !functions.contains( node.name ) )
        ENGINE_PANIC( L"function [", node.name, L"] doesn't exist" );

    auto& func = functions.at( node.name );
    handle_func( func, node.args, retval );
}

void dawn::Engine::handle_return_node( ReturnNode const& node, ValueBox& retval, Bool& didret )
{
    handle_expr( node.expr, retval );
    didret = true;
}

void dawn::Engine::handle_break_node( BreakNode const& node, Bool* didbrk )
{
    if ( !didbrk )
        ENGINE_PANIC( "break statement outside of loop" );
    *didbrk = true;
}

void dawn::Engine::handle_continue_node( ContinueNode const& node, Bool* didcon )
{
    if ( !didcon )
        ENGINE_PANIC( "continue statement outside of loop" );
    *didcon = true;
}

void dawn::Engine::handle_if_node( IfNode const& node, ValueBox& retval, Bool& didret, Bool* didbrk, Bool* didcon )
{
    ValueBox check_val;
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

void dawn::Engine::handle_switch_node( SwitchNode const& node, ValueBox& retval, Bool& didret, Bool* didbrk, Bool* didcon )
{
    ValueBox check_val;
    handle_expr( node.main_expr, check_val );

    for ( auto& case_part : node.cases )
    {
        for ( auto& expr : case_part.exprs )
        {
            ValueBox case_val;
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

void dawn::Engine::handle_loop_node( LoopNode const& node, ValueBox& retval, Bool& didret )
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

void dawn::Engine::handle_while_node( WhileNode const& node, ValueBox& retval, Bool& didret )
{
    Bool didbrk = false, didcon = false;
    while ( true )
    {
        ValueBox check_val;
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

void dawn::Engine::handle_for_node( ForNode const& node, ValueBox& retval, Bool& didret )
{
    ValueBox loop_val;
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

            add_var( node.var, value );

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

void dawn::Engine::handle_enum_node( EnumNode const& node, ValueBox& value )
{
    if ( !enums.contains( node.type ) )
        ENGINE_PANIC( "enum [", node.type, L"] doesn't exist" );

    EnumVal result{};
    result.parent = &enums.at( node.type );
    result.key = node.key;

    value = ValueBox{ result };
}

void dawn::Engine::handle_struct_node( StructNode const& node, ValueBox& value )
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

        ValueBox field_val;
        handle_expr( struc_field.expr, field_val );
        field = ValueBox{ field_val.value() };
    }

    for ( auto& [arg_name, arg_expr] : node.args )
    {
        ValueBox arg_val;
        handle_expr( arg_expr, arg_val );

        if ( !result.members.contains( arg_name ) )
            ENGINE_PANIC( "field [", arg_name, L"] doesn't exist in struct [", node.type, L"]" );

        result.members.at( arg_name ) = ValueBox{ arg_val.value() };
    }

    value = ValueBox{ result };
}

void dawn::Engine::handle_array_node( ArrayNode const& node, ValueBox& value )
{
    ArrayVal result{};

    if ( node.init_type == ArrayNode::InitType::SIZE )
    {
        ValueBox size_val;
        handle_expr( node.SIZE_size_expr, size_val );

        Int size = size_val.value().to_int();
        if ( size < 0 )
            ENGINE_PANIC( "Array size cannot be negative" );

        ValueBox value_val;
        handle_expr( node.SIZE_value_expr, value_val );

        result.data.resize( size );
        for ( auto& val : result.data )
            val = ValueBox{ value_val.value() };
    }
    else
    {
        result.data.reserve( node.LIST_list.size() );
        for ( auto& expr : node.LIST_list )
        {
            ValueBox entry_val;
            handle_expr( expr, entry_val );
            result.data.emplace_back( entry_val.value() );
        }
    }

    value = ValueBox{ result };
}

void dawn::Engine::handle_un_node( UnaryNode const& node, ValueBox& value )
{
    ValueBox right_val;
    handle_expr( node.right, right_val );

    switch ( node.type )
    {
    case UnaryType::PLUS:
        value = ValueBox{ +right_val.value() };
        break;

    case UnaryType::MINUS:
        value = ValueBox{ -right_val.value() };
        break;

    case UnaryType::NOT:
        value = ValueBox{ !right_val.value() };
        break;

    case UnaryType::RANGE:
        value = ValueBox{ ~right_val.value() };
        break;

    default:
        ENGINE_PANIC( "Unknown unary node type: ", typeid(node).name() );
    }
}

void dawn::Engine::handle_op_node( OperatorNode const& node, ValueBox& value )
{
    if ( OperatorType::ACCESS == node.type )
        return handle_ac_node( node, value );

    ValueBox left_val;
    handle_expr( node.left, left_val );

    ValueBox right_val;
    handle_expr( node.right, right_val );

    switch ( node.type )
    {
    case OperatorType::ADD:
        value = ValueBox{ left_val.value() + right_val.value() };
        break;

    case OperatorType::SUB:
        value = ValueBox{ left_val.value() - right_val.value() };
        break;

    case OperatorType::MUL:
        value = ValueBox{ left_val.value() * right_val.value() };
        break;

    case OperatorType::DIV:
        value = ValueBox{ left_val.value() / right_val.value() };
        break;

    case OperatorType::POW:
        value = ValueBox{ left_val.value() ^ right_val.value() };
        break;

    case OperatorType::MOD:
        value = ValueBox{ left_val.value() % right_val.value() };
        break;

    case OperatorType::AND:
        value = ValueBox{ left_val.value() && right_val.value() };
        break;

    case OperatorType::OR:
        value = ValueBox{ left_val.value() || right_val.value() };
        break;

    case OperatorType::EQ:
        value = ValueBox{ left_val.value() == right_val.value() };
        break;

    case OperatorType::NOT_EQ:
        value = ValueBox{ left_val.value() != right_val.value() };
        break;

    case OperatorType::LESS:
        value = ValueBox{ left_val.value() < right_val.value() };
        break;

    case OperatorType::GREAT:
        value = ValueBox{ left_val.value() > right_val.value() };
        break;

    case OperatorType::LESS_EQ:
        value = ValueBox{ left_val.value() <= right_val.value() };
        break;

    case OperatorType::GREAT_EQ:
        value = ValueBox{ left_val.value() >= right_val.value() };
        break;

    case OperatorType::RANGE:
        value = ValueBox{ left_val.value() >> right_val.value() };
        break;

    default:
        ENGINE_PANIC( "Unknown operator node type: ", typeid(node).name() );
    }
}

void dawn::Engine::handle_ac_node( OperatorNode const& node, ValueBox& value )
{
    ValueBox left_val;
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

void dawn::Engine::handle_as_node( AssignNode const& node, ValueBox& value )
{
    ValueBox left_val;
    handle_expr( node.left, left_val );

    ValueBox right_val;
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

void dawn::Engine::handle_ac_string_node( ValueBox const& left, Ref<Node> const& right, ValueBox& value )
{
    auto& left_val = left.value().as<String>();

    if ( auto id_node = dynamic_cast<IdentifierNode const*>(right.get()) )
    {
        if ( id_node->name == L"length" )
        {
            value = ValueBox{ Value{ (Int) left_val.size() } };
            return;
        }
    }

    ValueBox right_val;
    handle_expr( right, right_val );

    Int index = right_val.value().to_int();
    if ( index < 0 || index >= (Int) left_val.size() )
        ENGINE_PANIC( "String access [", index, "] out of bounds" );

    value = ValueBox{ Value{ left_val[index] } };
}

void dawn::Engine::handle_ac_enum_node( ValueBox const& left, Ref<Node> const& right, ValueBox& value )
{
    auto& left_val = left.value().as<EnumVal>();

    if ( auto id_node = dynamic_cast<IdentifierNode const*>(right.get()) )
    {
        if ( id_node->name == L"value" )
        {
            value = left_val.parent->keys_value.at( left_val.key );
            return;
        }

        ENGINE_PANIC( "Enum access [", id_node->name, "] doesn't exist" );
    }

    ENGINE_PANIC( "Enum access must be an identifier" );
}

void dawn::Engine::handle_ac_struct_node( ValueBox const& left, Ref<Node> const& right, ValueBox& value )
{
    auto& left_val = left.value().as<StructVal>();

    if ( auto id_node = dynamic_cast<IdentifierNode const*>(right.get()) )
    {
        if ( !left_val.members.contains( id_node->name ) )
            ENGINE_PANIC( "Member [", id_node->name, "] doesn't exist" );

        value = left_val.members.at( id_node->name );
        return;
    }

    if ( auto func_node = dynamic_cast<FunctionNode const*>(right.get()) )
    {
        auto method_ptr = left_val.parent->get_method( func_node->name );
        if ( !method_ptr )
            ENGINE_PANIC( "Method [", func_node->name, "] doesn't exist" );

        auto self_node = std::make_shared<BoxNode>();
        self_node->box = left;

        Array<Ref<Node>> args = { self_node };
        args.insert( args.end(), func_node->args.begin(), func_node->args.end() );

        handle_func( *method_ptr, args, value );
        return;
    }

    ENGINE_PANIC( "Struct access must be an identifier or function call" );
}

void dawn::Engine::handle_ac_array_node( ValueBox const& left, Ref<Node> const& right, ValueBox& value )
{
    auto& left_val = left.value().as<ArrayVal>();

    if ( auto id_node = dynamic_cast<IdentifierNode const*>(right.get()) )
    {
        if ( id_node->name == L"length" )
        {
            value = ValueBox{ Value{ (Int) left_val.data.size() } };
            return;
        }

        ENGINE_PANIC( "Array access [", id_node->name, "] doesn't exist" );
    }

    ValueBox right_val;
    handle_expr( right, right_val );

    Int index = right_val.value().to_int();
    if ( index < 0 || index >= (Int) left_val.data.size() )
        ENGINE_PANIC( "Array access [", index, "] out of bounds" );

    value = left_val.data[index];
}
