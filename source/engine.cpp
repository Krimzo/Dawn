#include "engine.h"


std::wostream& dawn::operator<<( std::wostream& stream, EngineError const& error )
{
    stream << error.msg;
    return stream;
}

dawn::Opt<dawn::EngineError> dawn::Engine::load_mod( Module const& module )
{
    try
    {
        auto helper_func = [this]( auto& out_coll, auto const& in_coll )
        {
            for ( auto& entry : in_coll )
                out_coll.push( entry.name, entry );
        };

        helper_func( enums, module.enums );
        helper_func( structs, module.structs );
        helper_func( functions, module.functions );

        for ( auto& entry : module.variables )
        {
            if ( auto error = add_var( entry ) )
                return error;
        }

        return std::nullopt;
    }
    catch ( String const& msg )
    {
        return msg;
    }
}

void dawn::Engine::bind_func( String const& name, Function::CppFunc cpp_func )
{
    Function func;
    func.name = name;
    func.body.emplace<Function::CppFunc>( std::move( cpp_func ) );
    functions.push( name, func );
}

dawn::Opt<dawn::EngineError> dawn::Engine::call_func( String const& name, Array<Ref<Node>> const& args, ValueBox& retval )
{
    try
    {
        auto* func = functions.get( name );
        if ( !func )
            return EngineError{ L"function [", name, L"] doesn't exist" };

        if ( auto error = handle_func( *func, args, retval ) )
            return error;

        return std::nullopt;
    }
    catch ( String const& msg )
    {
        return msg;
    }
}

void dawn::Engine::add_var( String const& name, Bool is_var, RawValue const& value )
{
    ValueBox var_val = { is_var ? ValueBox::Type::VAR : ValueBox::Type::LET, value };
    variables.push( name, var_val );
}

void dawn::Engine::add_var( Variable const& var, ValueBox const& value )
{
    if ( var.kind == Variable::Kind::LET )
    {
        variables.push( var.name, ValueBox{ ValueBox::Type::LET, value.value() } );
    }
    else if ( var.kind == Variable::Kind::VAR )
    {
        variables.push( var.name, ValueBox{ ValueBox::Type::VAR, value.value() } );
    }
    else
    {
        variables.push( var.name, value );
    }
}

dawn::Opt<dawn::EngineError> dawn::Engine::add_var( Variable const& var )
{
    try
    {
        ValueBox var_val;
        if ( auto error = handle_expr( var.expr, var_val ) )
            return error;

        add_var( var, var_val );

        return std::nullopt;
    }
    catch ( String const& msg )
    {
        return msg;
    }
}

dawn::ValueBox* dawn::Engine::get_var( String const& name )
{
    return variables.get( name );
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_func( Function const& func, Array<Ref<Node>> const& args, ValueBox& retval )
{
    if ( func.body.index() == 1 )
    {
        Array<ValueBox> arg_vals;
        for ( auto& arg : args )
        {
            ValueBox arg_val;
            if ( auto error = handle_expr( arg, arg_val ) )
                return error;

            arg_vals.push_back( arg_val );
        }

        auto result = std::get<Function::CppFunc>( func.body )(arg_vals);
        if ( result )
            retval = ValueBox{ ValueBox::Type::LET, result };

        return std::nullopt;
    }

    if ( func.args.size() != args.size() )
        return EngineError{ "invalid argument count for function [", func.name, L"]" };

    for ( Int i = 0; i < (Int) args.size(); i++ )
    {
        Variable arg;
        arg.name = func.args[i].name;
        arg.kind = func.args[i].kind;
        arg.expr = args[i];

        if ( auto error = add_var( arg ) )
            return error;
    }

    Bool didret = false;
    if ( auto error = handle_scope( std::get<Scope>( func.body ), retval, didret, nullptr, nullptr ) )
        return error;

    variables.pop( (Int) args.size() );

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_scope( Scope const& scope, ValueBox& retval, Bool& didret, Bool* didbrk, Bool* didcon )
{
    Int push_count = 0;

    for ( auto& instr : scope.instr )
    {
        if ( didret )
            break;
        if ( didbrk && *didbrk )
            break;
        if ( didcon && *didcon )
            break;
        if ( auto error = handle_instr( instr, retval, push_count, didret, didbrk, didcon ) )
            return error;
    }

    variables.pop( push_count );

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_instr( Ref<Node> const& node, ValueBox& retval, Int& push_count, Bool& didret, Bool* didbrk, Bool* didcon )
{
    if ( auto scp = dynamic_cast<Scope*>(node.get()) )
        return handle_scope( *scp, retval, didret, didbrk, didcon );

    if ( auto nd = dynamic_cast<VariableNode*>(node.get()) )
        return handle_var_node( *nd, push_count );

    if ( auto nd = dynamic_cast<ReturnNode*>(node.get()) )
        return handle_return_node( *nd, retval, didret );

    if ( auto nd = dynamic_cast<BreakNode*>(node.get()) )
        return handle_break_node( *nd, didbrk );

    if ( auto nd = dynamic_cast<ContinueNode*>(node.get()) )
        return handle_continue_node( *nd, didcon );

    if ( auto nd = dynamic_cast<IfNode*>(node.get()) )
        return handle_if_node( *nd, retval, didret, didbrk, didcon );

    if ( auto nd = dynamic_cast<SwitchNode*>(node.get()) )
        return handle_switch_node( *nd, retval, didret, didbrk, didcon );

    if ( auto nd = dynamic_cast<LoopNode*>(node.get()) )
        return handle_loop_node( *nd, retval, didret );

    if ( auto nd = dynamic_cast<WhileNode*>(node.get()) )
        return handle_while_node( *nd, retval, didret );

    if ( auto nd = dynamic_cast<ForNode*>(node.get()) )
        return handle_for_node( *nd, retval, didret );

    if ( auto error = handle_expr( node, retval ) )
        return error;

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_expr( Ref<Node> const& node, ValueBox& value )
{
    if ( auto nd = dynamic_cast<ValueNode*>(node.get()) )
        return handle_val_node( *nd, value );

    if ( auto nd = dynamic_cast<ArrayNode*>(node.get()) )
        return handle_array_node( *nd, value );

    if ( auto nd = dynamic_cast<StructNode*>(node.get()) )
        return handle_struct_node( *nd, value );

    if ( auto nd = dynamic_cast<CastNode*>(node.get()) )
        return handle_cast_node( *nd, value );

    if ( auto nd = dynamic_cast<IdentifierNode*>(node.get()) )
        return handle_id_node( *nd, value );

    if ( auto nd = dynamic_cast<FunctionNode*>(node.get()) )
        return handle_func_node( *nd, value );

    if ( auto nd = dynamic_cast<UnaryNode*>(node.get()) )
        return handle_un_node( *nd, value );

    if ( auto nd = dynamic_cast<OperatorNode*>(node.get()) )
        return handle_op_node( *nd, value );

    return EngineError{ "Unknown expr node type: ", typeid(*node).name() };
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_val_node( ValueNode const& node, ValueBox& value )
{
    value = ValueBox{ ValueBox::Type::LET, node.value };

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_array_node( ArrayNode const& node, ValueBox& value )
{
    auto result = std::make_shared<ArrayValue>();

    if ( node.init_type == ArrayNode::InitType::SIZE )
    {
        ValueBox size_val;
        if ( auto error = handle_expr( node.SIZE_size_expr, size_val ) )
            return error;

        Int size = size_val.value()->to_int();
        if ( size < 0 )
            return EngineError{ "Array size cannot be negative" };

        ValueBox value_val;
        if ( auto error = handle_expr( node.SIZE_value_expr, value_val ) )
            return error;

        result->data.resize( size );
        for ( auto& val : result->data )
            val = ValueBox{ ValueBox::Type::VAR, value_val.value()->clone() };
    }
    else
    {
        result->data.reserve( node.LIST_list.size() );
        for ( auto& expr : node.LIST_list )
        {
            ValueBox entry_val;
            if ( auto error = handle_expr( expr, entry_val ) )
                return error;

            result->data.emplace_back( ValueBox::Type::VAR, entry_val.value() );
        }
    }

    value = ValueBox{ ValueBox::Type::LET, result };

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_struct_node( StructNode const& node, ValueBox& value )
{
    Struct* struc = structs.get( node.type );
    if ( !struc )
        return EngineError{ "struct [", node.type, L"] doesn't exist" };

    auto result = std::make_shared<StructValue>();
    result->parent = struc;

    for ( auto& struc_field : struc->fields )
    {
        auto& field = result->members[struc_field.name];
        if ( node.args.contains( struc_field.name ) )
            continue;

        ValueBox field_val;
        if ( auto error = handle_expr( struc_field.expr, field_val ) )
            return error;

        field = ValueBox{ ValueBox::Type::VAR, field_val.value() };
    }

    for ( auto& [arg_name, arg_expr] : node.args )
    {
        ValueBox arg_val;
        if ( auto error = handle_expr( arg_expr, arg_val ) )
            return error;

        if ( !result->members.contains( arg_name ) )
            return EngineError{ "field [", arg_name, L"] doesn't exist in struct [", node.type, L"]" };

        result->members.at( arg_name ) = ValueBox{ ValueBox::Type::VAR, arg_val.value() };
    }

    value = ValueBox{ ValueBox::Type::LET, result };

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_cast_node( CastNode const& node, ValueBox& value )
{
    ValueBox cast_val;
    if ( auto error = handle_expr( node.expr, cast_val ) )
        return error;

    if ( node.type == tp_bool )
    {
        value = ValueBox{ ValueBox::Type::LET, make_bool_value( cast_val.value()->to_bool() ) };
    }
    else if ( node.type == tp_int )
    {
        value = ValueBox{ ValueBox::Type::LET, make_int_value( cast_val.value()->to_int() ) };
    }
    else if ( node.type == tp_float )
    {
        value = ValueBox{ ValueBox::Type::LET, make_float_value( cast_val.value()->to_float() ) };
    }
    else if ( node.type == tp_char )
    {
        value = ValueBox{ ValueBox::Type::LET, make_char_value( cast_val.value()->to_char() ) };
    }
    else if ( node.type == tp_string )
    {
        value = ValueBox{ ValueBox::Type::LET, make_string_value( cast_val.value()->to_string() ) };
    }
    else
        return EngineError{ "Unknown cast type: ", node.type };

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_var_node( VariableNode const& node, Int& push_count )
{
    if ( auto error = add_var( node.var ) )
        return error;

    ++push_count;

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_id_node( IdentifierNode const& node, ValueBox& value )
{
    auto* ptr = variables.get( node.name );
    if ( !ptr )
        return EngineError{ L"variable [", node.name, L"] doesn't exist" };

    value = *ptr;

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_func_node( FunctionNode const& node, ValueBox& retval )
{
    auto* func = functions.get( node.name );
    if ( !func )
        return EngineError{ L"function [", node.name, L"] doesn't exist" };

    return handle_func( *func, node.args, retval );
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_return_node( ReturnNode const& node, ValueBox& retval, Bool& didret )
{
    if ( auto error = handle_expr( node.expr, retval ) )
        return error;

    didret = true;

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_break_node( BreakNode const& node, Bool* didbrk )
{
    if ( !didbrk )
        return EngineError{ "break statement outside of loop" };

    *didbrk = true;

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_continue_node( ContinueNode const& node, Bool* didcon )
{
    if ( !didcon )
        return EngineError{ "continue statement outside of loop" };

    *didcon = true;

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_if_node( IfNode const& node, ValueBox& retval, Bool& didret, Bool* didbrk, Bool* didcon )
{
    ValueBox check_val;
    if ( auto error = handle_expr( node.if_part.expr, check_val ) )
        return error;

    if ( check_val.value()->to_bool() )
        return handle_scope( node.if_part.scope, retval, didret, didbrk, didcon );

    for ( auto& elif_part : node.elif_parts )
    {
        if ( auto error = handle_expr( elif_part.expr, check_val ) )
            return error;
        if ( check_val.value()->to_bool() )
            return handle_scope( elif_part.scope, retval, didret, didbrk, didcon );
    }

    if ( node.else_part )
        return handle_scope( node.else_part->scope, retval, didret, didbrk, didcon );

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_switch_node( SwitchNode const& node, ValueBox& retval, Bool& didret, Bool* didbrk, Bool* didcon )
{
    ValueBox check_val;
    if ( auto error = handle_expr( node.main_expr, check_val ) )
        return error;

    for ( auto& case_part : node.cases )
    {
        for ( auto& expr : case_part.exprs )
        {
            ValueBox case_val;
            if ( auto error = handle_expr( expr, case_val ) )
                return error;

            if ( (*check_val.value() == *case_val.value())->to_bool() )
            {
                if ( auto error = handle_scope( case_part.scope, retval, didret, didbrk, didcon ) )
                    return error;
                goto func_end;
            }
        }
    }

    if ( node.def_scope )
    {
        if ( auto error = handle_scope( *node.def_scope, retval, didret, didbrk, didcon ) )
            return error;
    }

func_end:

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_loop_node( LoopNode const& node, ValueBox& retval, Bool& didret )
{
    Bool didbrk = false, didcon = false;
    while ( true )
    {
        if ( didret || didbrk )
            break;

        if ( didcon )
            didcon = false;

        if ( auto error = handle_scope( node.scope, retval, didret, &didbrk, &didcon ) )
            return error;
    }

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_while_node( WhileNode const& node, ValueBox& retval, Bool& didret )
{
    Bool didbrk = false, didcon = false;
    while ( true )
    {
        ValueBox check_val;
        if ( auto error = handle_expr( node.expr, check_val ) )
            return error;

        if ( !check_val.value()->to_bool() )
            break;

        if ( didret || didbrk )
            break;

        if ( didcon )
            didcon = false;

        if ( auto error = handle_scope( node.scope, retval, didret, &didbrk, &didcon ) )
            return error;
    }

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_for_node( ForNode const& node, ValueBox& retval, Bool& didret )
{
    ValueBox loop_val;
    if ( auto error = handle_expr( node.expr, loop_val ) )
        return error;

    if ( auto value_rng = dynamic_cast<RangeValue const*>(loop_val.value().get()) )
    {
        Bool didbrk = false, didcon = false;
        for ( auto i = value_rng->start_incl; i < value_rng->end_excl; ++i )
        {
            if ( didret || didbrk )
                break;

            if ( didcon )
                didcon = false;

            Variable arg = node.var;
            arg.expr = make_int_node( i );

            if ( auto error = add_var( arg ) )
                return error;

            if ( auto error = handle_scope( node.scope, retval, didret, &didbrk, &didcon ) )
                return error;

            variables.pop();
        }
    }
    else if ( auto value_str = dynamic_cast<StringValue const*>(loop_val.value().get()) )
    {
        Bool didbrk = false, didcon = false;
        for ( Char c : value_str->value )
        {
            if ( didret || didbrk )
                break;

            if ( didcon )
                didcon = false;

            Variable arg = node.var;
            arg.expr = make_char_node( c );

            if ( auto error = add_var( arg ) )
                return error;

            if ( auto error = handle_scope( node.scope, retval, didret, &didbrk, &didcon ) )
                return error;

            variables.pop();
        }
    }
    else if ( auto value_arr = dynamic_cast<ArrayValue const*>(loop_val.value().get()) )
    {
        Bool didbrk = false, didcon = false;
        for ( auto& value : value_arr->data )
        {
            if ( didret || didbrk )
                break;

            if ( didcon )
                didcon = false;

            add_var( node.var, value );

            if ( auto error = handle_scope( node.scope, retval, didret, &didbrk, &didcon ) )
                return error;

            variables.pop();
        }
    }
    else
        return EngineError{ "Can't for loop [", loop_val.value()->type(), "]" };

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_un_node( UnaryNode const& node, ValueBox& value )
{
    ValueBox right_val;
    if ( auto error = handle_expr( node.right, right_val ) )
        return error;

    if ( typeid(node) == typeid(UnaryNodePlus) )
        value = ValueBox{ ValueBox::Type::LET, right_val.value()->clone() };
    else if ( typeid(node) == typeid(UnaryNodeMinus) )
        value = ValueBox{ ValueBox::Type::LET, -(*right_val.value()) };
    else if ( typeid(node) == typeid(UnaryNodeNot) )
        value = ValueBox{ ValueBox::Type::LET, !(*right_val.value()) };
    else if ( typeid(node) == typeid(UnaryNodeRange) )
        value = ValueBox{ ValueBox::Type::LET, ~(*right_val.value()) };
    else
        return EngineError{ "Unknown unary node type: ", typeid(node).name() };

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_op_node( OperatorNode const& node, ValueBox& value )
{
    if ( auto nd = dynamic_cast<OperatorNodeAccess const*>(&node) )
        return handle_ac_node( *nd, value );

    if ( auto nd = dynamic_cast<AssignNode const*>(&node) )
        return handle_as_node( *nd, value );

    ValueBox left_val;
    if ( auto error = handle_expr( node.left, left_val ) )
        return error;

    ValueBox right_val;
    if ( auto error = handle_expr( node.right, right_val ) )
        return error;

    if ( typeid(node) == typeid(OperatorNodeAdd) )
        value = ValueBox{ ValueBox::Type::LET, (*left_val.value()) + (*right_val.value()) };
    else if ( typeid(node) == typeid(OperatorNodeSub) )
        value = ValueBox{ ValueBox::Type::LET, (*left_val.value()) - (*right_val.value()) };
    else if ( typeid(node) == typeid(OperatorNodeMul) )
        value = ValueBox{ ValueBox::Type::LET, (*left_val.value()) * (*right_val.value()) };
    else if ( typeid(node) == typeid(OperatorNodeDiv) )
        value = ValueBox{ ValueBox::Type::LET, (*left_val.value()) / (*right_val.value()) };
    else if ( typeid(node) == typeid(OperatorNodePow) )
        value = ValueBox{ ValueBox::Type::LET, (*left_val.value()) ^ (*right_val.value()) };
    else if ( typeid(node) == typeid(OperatorNodeMod) )
        value = ValueBox{ ValueBox::Type::LET, (*left_val.value()) % (*right_val.value()) };
    else if ( typeid(node) == typeid(OperatorNodeAnd) )
        value = ValueBox{ ValueBox::Type::LET, (*left_val.value()) && (*right_val.value()) };
    else if ( typeid(node) == typeid(OperatorNodeOr) )
        value = ValueBox{ ValueBox::Type::LET, (*left_val.value()) || (*right_val.value()) };
    else if ( typeid(node) == typeid(OperatorNodeEq) )
        value = ValueBox{ ValueBox::Type::LET, (*left_val.value()) == (*right_val.value()) };
    else if ( typeid(node) == typeid(OperatorNodeNotEq) )
        value = ValueBox{ ValueBox::Type::LET, (*left_val.value()) != (*right_val.value()) };
    else if ( typeid(node) == typeid(OperatorNodeLess) )
        value = ValueBox{ ValueBox::Type::LET, (*left_val.value()) < (*right_val.value()) };
    else if ( typeid(node) == typeid(OperatorNodeGreat) )
        value = ValueBox{ ValueBox::Type::LET, (*left_val.value()) > (*right_val.value()) };
    else if ( typeid(node) == typeid(OperatorNodeLessEq) )
        value = ValueBox{ ValueBox::Type::LET, (*left_val.value()) <= (*right_val.value()) };
    else if ( typeid(node) == typeid(OperatorNodeGreatEq) )
        value = ValueBox{ ValueBox::Type::LET, (*left_val.value()) >= (*right_val.value()) };
    else if ( typeid(node) == typeid(OperatorNodeRange) )
        value = ValueBox{ ValueBox::Type::LET, (*left_val.value()) >> (*right_val.value()) };
    else
        return EngineError{ "Unknown operator node type: ", typeid(node).name() };

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_ac_node( OperatorNodeAccess const& node, ValueBox& value )
{
    ValueBox left_val;
    if ( auto error = handle_expr( node.left, left_val ) )
        return error;

    auto struc_val = dynamic_cast<StructValue const*>(left_val.value().get());
    if ( !struc_val )
        return EngineError{ "Can't access member of [", left_val.value()->type(), "]" };

    if ( auto id_node = dynamic_cast<IdentifierNode const*>(node.right.get()) )
    {
        if ( !struc_val->members.contains( id_node->name ) )
            return EngineError{ "Member [", id_node->name, "] doesn't exist" };

        value = struc_val->members.at( id_node->name );
    }
    else if ( auto func_node = dynamic_cast<FunctionNode const*>(node.right.get()) )
    {
        auto method_ptr = struc_val->parent->get_method( func_node->name );
        if ( !method_ptr )
            return EngineError{ "Method [", func_node->name, "] doesn't exist" };

        Array<Ref<Node>> args = { make_value_node( left_val.value() ) };
        args.insert( args.end(), func_node->args.begin(), func_node->args.end() );

        if ( auto error = handle_func( *method_ptr, args, value ) )
            return error;
    }
    else
        return EngineError{ "Struct access must be an identifier or function call" };

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_as_node( AssignNode const& node, ValueBox& value )
{
    ValueBox left_val;
    if ( auto error = handle_expr( node.left, left_val ) )
        return error;

    ValueBox right_val;
    if ( auto error = handle_expr( node.right, right_val ) )
        return error;

    if ( typeid(node) == typeid(AssignNodeAdd) )
    {
        left_val.set_value( *left_val.value() + (*right_val.value()) );
    }
    else if ( typeid(node) == typeid(AssignNodeSub) )
    {
        left_val.set_value( *left_val.value() - (*right_val.value()) );
    }
    else if ( typeid(node) == typeid(AssignNodeMul) )
    {
        left_val.set_value( *left_val.value() * (*right_val.value()) );
    }
    else if ( typeid(node) == typeid(AssignNodeDiv) )
    {
        left_val.set_value( *left_val.value() / (*right_val.value()) );
    }
    else if ( typeid(node) == typeid(AssignNodePow) )
    {
        left_val.set_value( *left_val.value() ^ (*right_val.value()) );
    }
    else if ( typeid(node) == typeid(AssignNodeMod) )
    {
        left_val.set_value( *left_val.value() % (*right_val.value()) );
    }
    else
    {
        left_val.set_value( right_val.value() );
    }

    value = left_val;

    return std::nullopt;
}
