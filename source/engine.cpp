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

    if ( auto error = handle_scope( func.body, retval ) )
        return error;

    m_variables.pop( (Int) args.size() );

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_scope( Scope const& scope, Ref<Value>& retval )
{
    Int push_count = 0;

    for ( auto& instr : scope.instr )
    {
        Bool didbrk = false;
        if ( auto error = handle_instr( instr, retval, push_count, didbrk ) )
            return error;
        if ( didbrk )
            break;
    }

    m_variables.pop( push_count );

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_instr( Ref<Node> const& node, Ref<Value>& retval, Int& push_count, Bool& didbrk )
{
    if ( auto nd = dynamic_cast<NewVarNode*>(node.get()) )
        return handle_new_var_instr( *nd, push_count );

    if ( auto nd = dynamic_cast<ReturnNode*>(node.get()) )
    {
        if ( auto error = handle_expr( nd->expr, retval ) )
            return error;

        didbrk = true;
        return std::nullopt;
    }

    Ref<Value> _val;
    if ( auto error = handle_expr( node, _val ) )
        return error;

    return std::nullopt;
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

    if ( auto scp = dynamic_cast<Scope*>(node.get()) )
        return handle_scope( *scp, value );

    return EngineError{ "Unknown expr node type: ", typeid(*node).name() };
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
    String result;
    for ( auto& arg : node.args )
    {
        Ref<Value> val;
        if ( auto error = handle_expr( arg, val ) )
            return error;
        result += val->to_string();
    }

    print( result );

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_un_node( UnaryNode const& node, Ref<Value>& value )
{
    Ref<Value> right;
    if ( auto error = handle_expr( node.right, right ) )
        return error;

    if ( typeid(node) == typeid(UnaryNodePlus) )
        value = +(*right);
    else if ( typeid(node) == typeid(UnaryNodeMinus) )
        value = -(*right);
    //else if ( typeid(node) == typeid(UnaryNodeNot) )
    //    value = !(*right);
    //else if ( typeid(node) == typeid(UnaryNodeRef) )
    //    value = &(*right);
    //else if ( typeid(node) == typeid(UnaryNodeRange) )
    //    value = ~(*right);
    else
        return EngineError{ "Unknown unary node type: ", typeid(node).name() };

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_op_node( OperatorNode const& node, Ref<Value>& value )
{
    if ( auto nd = dynamic_cast<AssignNode const*>(&node) )
        return handle_as_node( *nd, value );

    Ref<Value> left;
    if ( auto error = handle_expr( node.left, left ) )
        return error;

    Ref<Value> right;
    if ( auto error = handle_expr( node.right, right ) )
        return error;

    if ( typeid(node) == typeid(OperatorNodeAdd) )
        value = (*left) + (*right);
    else if ( typeid(node) == typeid(OperatorNodeSub) )
        value = (*left) - (*right);
    else if ( typeid(node) == typeid(OperatorNodeMul) )
        value = (*left) * (*right);
    else if ( typeid(node) == typeid(OperatorNodeDiv) )
        value = (*left) / (*right);
    else if ( typeid(node) == typeid(OperatorNodePow) )
        value = (*left) ^ (*right);
    else if ( typeid(node) == typeid(OperatorNodeMod) )
        value = (*left) % (*right);
    else
        return EngineError{ "Unknown operator node type: ", typeid(node).name() };

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_as_node( AssignNode const& node, Ref<Value>& value )
{
    auto id = dynamic_cast<IdentifierNode const*>(node.left.get());
    if ( !id )
        return EngineError{ "left value of assign must be an identifier" };

    Ref<Value> right;
    if ( auto error = handle_expr( node.right, right ) )
        return error;

    auto var = m_variables.get( id->name );
    if ( !var )
        return EngineError{ "variable [", id->name, L"] doesn't exist" };

    if ( !var->is_var )
        return EngineError{ "variable [", id->name, L"] is a constant" };

    var->value = std::move( right );

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_new_var_instr( NewVarNode const& node, Int& push_count )
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

    ++push_count;

    return std::nullopt;
}
