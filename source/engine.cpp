#include "engine.h"


std::wostream& dawn::operator<<( std::wostream& stream, EngineError const& error )
{
    stream << error.msg;
    return stream;
}

dawn::Opt<dawn::EngineError> dawn::Engine::load_mod( Module const& module )
{
    auto helper_func = [this]( auto& out_coll, auto const& in_coll )
    {
        for ( auto& [key, part] : in_coll )
            out_coll.push( key, part );
    };

    helper_func( functions, module.functions );
    helper_func( enums, module.enums );
    helper_func( layers, module.layers );
    helper_func( structs, module.structs );

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
        variables.push( key, eng_var );
    }

    return std::nullopt;
}

void dawn::Engine::bind_func( String const& name, Function::CppFunc cpp_func )
{
    Function func;
    func.name = name;
    func.body.emplace<Function::CppFunc>( std::move( cpp_func ) );
    functions.push( name, func );
}

dawn::Opt<dawn::EngineError> dawn::Engine::call_func( String const& name, Array<Ref<Value>> const& args, Ref<Value>& retval )
{
    auto* func = functions.get( name );
    if ( !func )
        return EngineError{ L"function [", name, L"] doesn't exist" };

    if ( auto error = handle_func( *func, args, retval ) )
        return error;

    return std::nullopt;
}

void dawn::Engine::set_var( String const& name, Ref<Value> const& value )
{
    EngineVar var;
    var.name = name;
    var.is_var = true;
    var.type;
    var.value = value->clone();
    variables.push( name, var );
}

dawn::Ref<dawn::Value> dawn::Engine::get_var( String const& name )
{
    auto* ptr = variables.get( name );
    return ptr ? ptr->value : nullptr;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_func( Function const& func, Array<Ref<Value>> const& args, Ref<Value>& retval )
{
    if ( func.body.index() == 1 )
    {
        try
        {
            retval = std::get<Function::CppFunc>( func.body )(args);
        }
        catch ( String& msg )
        {
            return EngineError{ msg };
        }

        return std::nullopt;
    }

    if ( func.args.size() != args.size() )
        return EngineError{ "invalid argument count for function [", func.name, L"]" };

    for ( Int i = 0; i < (Int) args.size(); i++ )
    {
        EngineVar arg;
        arg.name = func.args[i].name;
        arg.is_var = false;
        arg.type = func.args[i].type;
        arg.value = args[i];
        variables.push( arg.name, arg );
    }

    retval = nullptr;
    if ( auto error = handle_scope( std::get<Scope>( func.body ), retval ) )
        return error;

    variables.pop( (Int) args.size() );

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_scope( Scope const& scope, Ref<Value>& retval )
{
    Int push_count = 0;

    Bool didbrk = false;
    for ( auto& instr : scope.instr )
    {
        if ( didbrk || retval )
            break;
        if ( auto error = handle_instr( instr, retval, push_count, didbrk ) )
            return error;
    }

    variables.pop( push_count );

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_instr( Ref<Node> const& node, Ref<Value>& retval, Int& push_count, Bool& didbrk )
{
    if ( auto scp = dynamic_cast<Scope*>(node.get()) )
        return handle_scope( *scp, retval );

    if ( auto nd = dynamic_cast<VariableNode*>(node.get()) )
        return handle_var_instr( *nd, push_count );

    if ( auto nd = dynamic_cast<IfNode*>(node.get()) )
        return handle_if_node( *nd, retval );

    if ( auto nd = dynamic_cast<ReturnNode*>(node.get()) )
    {
        if ( auto error = handle_expr( nd->expr, retval ) )
            return error;

        return std::nullopt;
    }

    if ( auto error = handle_expr( node, retval ) )
        return error;

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_expr( Ref<Node> const& node, Ref<Value>& value )
{
    if ( auto nd = dynamic_cast<NothingNode*>(node.get()) )
        return handle_nothing_node( *nd, value );

    if ( auto nd = dynamic_cast<ValueNode*>(node.get()) )
        return handle_val_node( *nd, value );

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

dawn::Opt<dawn::EngineError> dawn::Engine::handle_nothing_node( NothingNode const& node, Ref<Value>& value )
{
    value = NothingValue::make();

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_val_node( ValueNode const& node, Ref<Value>& value )
{
    value = node.value;

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_cast_node( CastNode const& node, Ref<Value>& value )
{
    Ref<Value> expr;
    if ( auto error = handle_expr( node.expr, expr ) )
        return error;

    try
    {
        if ( node.type->name == tp_bool )
        {
            auto val = BoolValue::make();
            val->value = expr->to_bool();
            value = val;
        }
        else if ( node.type->name == tp_int )
        {
            auto val = IntValue::make();
            val->value = expr->to_int();
            value = val;
        }
        else if ( node.type->name == tp_float )
        {
            auto val = FloatValue::make();
            val->value = expr->to_float();
            value = val;
        }
        else if ( node.type->name == tp_char )
        {
            auto val = CharValue::make();
            val->value = expr->to_char();
            value = val;
        }
        else if ( node.type->name == tp_string )
        {
            auto val = StringValue::make();
            val->value = expr->to_string();
            value = val;
        }
        else
            return EngineError{ "Unknown cast type: ", node.type->name };
    }
    catch ( String& msg )
    {
        return EngineError{ msg };
    }

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_var_instr( VariableNode const& node, Int& push_count )
{
    Ref<Value> value;
    if ( auto error = handle_expr( node.var.expr, value ) )
        return error;

    EngineVar var;
    var.name = node.var.name;
    var.is_var = node.var.is_var;
    var.type = node.var.type;
    var.value = value;
    variables.push( node.var.name, var );

    ++push_count;

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_id_node( IdentifierNode const& node, Ref<Value>& value )
{
    auto* ptr = variables.get( node.name );
    if ( !ptr )
        return EngineError{ L"variable [", node.name, L"] doesn't exist" };

    value = ptr->value;

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_func_node( FunctionNode const& node, Ref<Value>& value )
{
    Array<Ref<Value>> args;
    for ( auto& arg : node.args )
    {
        Ref<Value> val;
        if ( auto error = handle_expr( arg, val ) )
            return error;
        args.emplace_back( std::move( val ) );
    }
    return call_func( node.name, args, value );
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_if_node( IfNode const& node, Ref<Value>& value )
{
    Ref<Value> check_expr;
    if ( auto error = handle_expr( node.if_part.expr, check_expr ) )
        return error;
    if ( check_expr->to_bool() )
        return handle_scope( node.if_part.scope, value );

    for ( auto& elif_part : node.elif_parts )
    {
        if ( auto error = handle_expr( elif_part.expr, check_expr ) )
            return error;
        if ( check_expr->to_bool() )
            return handle_scope( elif_part.scope, value );
    }

    if ( node.else_part )
        return handle_scope( node.else_part->scope, value );

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_un_node( UnaryNode const& node, Ref<Value>& value )
{
    Ref<Value> right;
    if ( auto error = handle_expr( node.right, right ) )
        return error;

    try
    {
        if ( typeid(node) == typeid(UnaryNodePlus) )
            value = right->clone();
        else if ( typeid(node) == typeid(UnaryNodeMinus) )
            value = -(*right);
        else if ( typeid(node) == typeid(UnaryNodeNot) )
            value = !(*right);
        else if ( typeid(node) == typeid(UnaryNodeRange) )
            value = ~(*right);
        else if ( typeid(node) == typeid(UnaryNodeRef) )
            value = right;
        else
            return EngineError{ "Unknown unary node type: ", typeid(node).name() };
    }
    catch ( String& msg )
    {
        return EngineError{ msg };
    }

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

    try
    {
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
        else if ( typeid(node) == typeid(OperatorNodeAnd) )
            value = (*left) && (*right);
        else if ( typeid(node) == typeid(OperatorNodeOr) )
            value = (*left) || (*right);
        else if ( typeid(node) == typeid(OperatorNodeEq) )
            value = (*left) == (*right);
        else if ( typeid(node) == typeid(OperatorNodeNotEq) )
            value = (*left) != (*right);
        else if ( typeid(node) == typeid(OperatorNodeLess) )
            value = (*left) < (*right);
        else if ( typeid(node) == typeid(OperatorNodeGreat) )
            value = (*left) > (*right);
        else if ( typeid(node) == typeid(OperatorNodeLessEq) )
            value = (*left) <= (*right);
        else if ( typeid(node) == typeid(OperatorNodeGreatEq) )
            value = (*left) >= (*right);
        else if ( typeid(node) == typeid(OperatorNodeRange) )
            value = (*left) >> (*right);
        else
            return EngineError{ "Unknown operator node type: ", typeid(node).name() };
    }
    catch ( String& msg )
    {
        return EngineError{ msg };
    }

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

    auto var = variables.get( id->name );
    if ( !var )
        return EngineError{ "variable [", id->name, L"] doesn't exist" };

    if ( !var->is_var )
        return EngineError{ "variable [", id->name, L"] is a constant" };

    if ( typeid(node) == typeid(AssignNodeAdd) )
        var->value = (*var->value) + (*right);
    else if ( typeid(node) == typeid(AssignNodeSub) )
        var->value = (*var->value) - (*right);
    else if ( typeid(node) == typeid(AssignNodeMul) )
        var->value = (*var->value) * (*right);
    else if ( typeid(node) == typeid(AssignNodeDiv) )
        var->value = (*var->value) / (*right);
    else if ( typeid(node) == typeid(AssignNodePow) )
        var->value = (*var->value) ^ (*right);
    else if ( typeid(node) == typeid(AssignNodeMod) )
        var->value = (*var->value) % (*right);
    else
        var->value = std::move( right );

    value = var->value;

    return std::nullopt;
}
