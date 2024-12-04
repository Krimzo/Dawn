#include "engine.h"


std::wostream& dawn::operator<<( std::wostream& stream, EngineError const& error )
{
    stream << error.msg;
    return stream;
}

dawn::Opt<dawn::EngineError> dawn::Engine::load( Module const& module )
{
    auto helper_func = [this]( auto& out_coll, auto const& in_coll )
    {
        for ( auto& [key, part] : in_coll )
            out_coll.push( key, part );
    };

    helper_func( m_functions, module.functions );
    helper_func( m_enums, module.enums );
    helper_func( m_layers, module.layers );
    helper_func( m_structs, module.structs );

    for ( auto& [key, var] : module.variables )
    {
        Ref<Value> value;
        if ( auto error = handle_expr( var.expr, value ) )
            return error;

        EngineVar eng_var;
        eng_var.name = var.name;
        eng_var.is_var = var.is_var;
        eng_var.type = var.type;
        eng_var.value = value;
        m_variables.push( key, eng_var );
    }

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::exec( String const& func_name, Array<Ref<Value>> const& args, Ref<Value>& retval, Bool allow_internal )
{
    auto* func = m_functions.get( func_name );
    if ( !func )
        return EngineError{ L"function [", func_name, L"] doesn't exist" };

    if ( auto error = handle_func( *func, args, retval ) )
        return error;

    return std::nullopt;
}

dawn::Bool dawn::Engine::get_global( String const& var_name, Ref<Value>& value )
{
    auto* ptr = m_variables.get( var_name );
    if ( !ptr )
        return false;

    value = ptr->value;
    return true;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_expr( Ref<Node> const& node, Ref<Value>& value )
{
    if ( auto nd = dynamic_cast<ValueNode*>(node.get()) )
        return handle_val_node( *nd, value );

    if ( auto nd = dynamic_cast<UnaryNode*>(node.get()) )
        return handle_un_node( *nd, value );

    if ( auto nd = dynamic_cast<OperatorNode*>(node.get()) )
        return handle_op_node( *nd, value );

    if ( auto nd = dynamic_cast<IdentifierNode*>(node.get()) )
        return handle_id_node( *nd, value );

    if ( auto nd = dynamic_cast<FunctionCallNode*>(node.get()) )
        return handle_func_call_node( *nd, value );

    if ( auto nd = dynamic_cast<PrintNode*>(node.get()) )
        return handle_print_node( *nd, value );

    return EngineError{ "Unknown expr node type: ", typeid(*node).name() };
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_instr( Ref<Node> const& node, Ref<Value>& retval, Bool& didret )
{
    if ( auto nd = dynamic_cast<NewVarNode*>(node.get()) )
        return handle_new_var_instr( *nd );

    if ( auto nd = dynamic_cast<ReturnNode*>(node.get()) )
    {
        if ( auto error = handle_expr( nd->expr, retval ) )
            return error;

        didret = true;
        return std::nullopt;
    }

    Ref<Value> _val;
    if ( auto error = handle_expr( node, _val ) )
        return error;

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_func( Function const& func, Array<Ref<Value>> const& args, Ref<Value>& retval )
{
    if ( func.args.size() != args.size() )
        return EngineError{ "invalid argument count" };

    for ( Int i = 0; i < (Int) args.size(); i++ )
    {
        EngineVar arg;
        arg.name = func.args[i].name;
        arg.is_var = false;
        arg.type = func.args[i].type;
        arg.value = args[i];
        m_variables.push( arg.name, arg );
    }

    for ( auto& instr : func.body.instr )
    {
        Bool didret = false;
        if ( auto error = handle_instr( instr, retval, didret ) )
            return error;
        if ( didret )
            break;
    }

    m_variables.pop( (Int) args.size() );

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_val_node( ValueNode const& node, Ref<Value>& value )
{
    value = node.value;

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_id_node( IdentifierNode const& node, Ref<Value>& value )
{
    auto* ptr = m_variables.get( node.name );
    if ( !ptr )
        return EngineError{ L"variable [", node.name, L"] doesn't exist" };

    value = ptr->value;
    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_func_call_node( FunctionCallNode const& node, Ref<Value>& value )
{
    Array<Ref<Value>> args;
    for ( auto& arg : node.args )
    {
        Ref<Value> val;
        if ( auto error = handle_expr( arg, val ) )
            return error;
        args.emplace_back( std::move( val ) );
    }
    return exec( node.name, args, value, true );
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_print_node( PrintNode const& node, Ref<Value>& value )
{
    Ref<Value> val;
    if ( auto error = handle_expr( node.expr, val ) )
        return error;

    print( val->to_string() );
    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_un_node( UnaryNode const& node, Ref<Value>& value )
{
    Ref<Value> right;
    if ( auto error = handle_expr( node.right, right ) )
        return error;

    if ( auto val = dynamic_cast<IntValue*>(right.get()) )
    {
        if ( auto error = handle_un_int( typeid(node), val->value, value ) )
            return error;
    }
    else if ( auto val = dynamic_cast<FloatValue*>(right.get()) )
    {
        if ( auto error = handle_un_float( typeid(node), val->value, value ) )
            return error;
    }
    else
    {
        return EngineError{ "unary operators support only ints and floats" };
    }

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_un_int( type_info const& node_type, Int right, Ref<Value>& value )
{
    if ( typeid(UnaryNodePlus) == node_type )
    {
        auto val = IntValue::make();
        val->value = +right;
        value = val;
        return std::nullopt;
    }

    if ( typeid(UnaryNodeMinus) == node_type )
    {
        auto val = IntValue::make();
        val->value = -right;
        value = val;
        return std::nullopt;
    }

    return EngineError{ "unsupported int unary operator" };
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_un_float( type_info const& node_type, Float right, Ref<Value>& value )
{
    if ( typeid(UnaryNodePlus) == node_type )
    {
        auto val = FloatValue::make();
        val->value = +right;
        value = val;
        return std::nullopt;
    }

    if ( typeid(UnaryNodeMinus) == node_type )
    {
        auto val = FloatValue::make();
        val->value = -right;
        value = val;
        return std::nullopt;
    }

    return EngineError{ "unsupported float unary operator" };
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_op_node( OperatorNode const& node, Ref<Value>& value )
{
    Ref<Value> right;
    if ( auto error = handle_expr( node.right, right ) )
        return error;

    Opt<Int> right_int;
    Opt<Float> right_float;

    if ( auto val = dynamic_cast<IntValue*>(right.get()) )
        right_int = val->value;
    else if ( auto val = dynamic_cast<FloatValue*>(right.get()) )
        right_float = val->value;
    else
        return EngineError{ "operators support only ints and floats" };

    if ( auto op_node = dynamic_cast<OperatorNodeAssign const*>(&node) )
    {
        auto id_node = dynamic_cast<IdentifierNode const*>(op_node->left.get());
        if ( !id_node )
            return EngineError{ "only identifiers can be assigned to" };

        auto var = m_variables.get( id_node->name );
        if ( !var )
            return EngineError{ L"variable [", id_node->name, L"] doesn't exist" };

        var->value = right;
        return std::nullopt;
    }

    Ref<Value> left;
    if ( auto error = handle_expr( node.left, left ) )
        return error;

    Opt<Int> left_int;
    Opt<Float> left_float;

    if ( auto val = dynamic_cast<IntValue*>(left.get()) )
        left_int = val->value;
    else if ( auto val = dynamic_cast<FloatValue*>(left.get()) )
        left_float = val->value;
    else
        return EngineError{ "operators support only ints and floats" };

    if ( left_int && right_int )
    {
        if ( auto error = handle_op_int( typeid(node), *left_int, *right_int, value ) )
            return error;
    }
    else
    {
        if ( left_int )
            left_float = (Float) *left_int;
        else if ( right_int )
            right_float = (Float) *right_int;

        if ( auto error = handle_op_float( typeid(node), *left_float, *right_float, value ) )
            return error;
    }

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_op_int( type_info const& node_type, Int left, Int right, Ref<Value>& value )
{
    if ( typeid(OperatorNodePow) == node_type )
    {
        auto val = IntValue::make();
        val->value = (Int) std::pow( left, right );
        value = val;
        return std::nullopt;
    }

    if ( typeid(OperatorNodeMod) == node_type )
    {
        auto val = IntValue::make();
        val->value = left % right;
        value = val;
        return std::nullopt;
    }

    if ( typeid(OperatorNodeMul) == node_type )
    {
        auto val = IntValue::make();
        val->value = left * right;
        value = val;
        return std::nullopt;
    }

    if ( typeid(OperatorNodeDiv) == node_type )
    {
        auto val = IntValue::make();
        val->value = left / right;
        value = val;
        return std::nullopt;
    }

    if ( typeid(OperatorNodeAdd) == node_type )
    {
        auto val = IntValue::make();
        val->value = left + right;
        value = val;
        return std::nullopt;
    }

    if ( typeid(OperatorNodeSub) == node_type )
    {
        auto val = IntValue::make();
        val->value = left - right;
        value = val;
        return std::nullopt;
    }

    return EngineError{ "unsupported int operator" };
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_op_float( type_info const& node_type, Float left, Float right, Ref<Value>& value )
{
    if ( typeid(OperatorNodePow) == node_type )
    {
        auto val = FloatValue::make();
        val->value = (Float) std::pow( left, right );
        value = val;
        return std::nullopt;
    }

    if ( typeid(OperatorNodeMod) == node_type )
    {
        auto val = FloatValue::make();
        val->value = (Float) std::remainder( left, right );
        value = val;
        return std::nullopt;
    }

    if ( typeid(OperatorNodeMul) == node_type )
    {
        auto val = FloatValue::make();
        val->value = left * right;
        value = val;
        return std::nullopt;
    }

    if ( typeid(OperatorNodeDiv) == node_type )
    {
        auto val = FloatValue::make();
        val->value = left / right;
        value = val;
        return std::nullopt;
    }

    if ( typeid(OperatorNodeAdd) == node_type )
    {
        auto val = FloatValue::make();
        val->value = left + right;
        value = val;
        return std::nullopt;
    }

    if ( typeid(OperatorNodeSub) == node_type )
    {
        auto val = FloatValue::make();
        val->value = left - right;
        value = val;
        return std::nullopt;
    }

    return EngineError{ "unsupported float operator" };
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_new_var_instr( NewVarNode const& node )
{
    Ref<Value> value;
    if ( auto error = handle_expr( node.var.expr, value ) )
        return error;

    EngineVar var;
    var.name = node.var.name;
    var.is_var = node.var.is_var;
    var.type = node.var.type;
    var.value = value;
    m_variables.push( node.var.name, var );

    return std::nullopt;
}
