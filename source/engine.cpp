#include "engine.h"


std::wostream& dawn::operator<<( std::wostream& stream, EngineError const& error )
{
    stream << "Error: " << error.msg;
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
    retval = NothingValue::make();

    if ( func.body.index() == 1 )
    {
        try
        {
            retval = std::get<Function::CppFunc>( func.body )(args);
            if ( !retval )
                retval = NothingValue::make();
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

    Bool didret = false;
    if ( auto error = handle_scope( std::get<Scope>( func.body ), retval, didret, nullptr, nullptr ) )
        return error;

    variables.pop( (Int) args.size() );

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_scope( Scope const& scope, Ref<Value>& retval, Bool& didret, Bool* didbrk, Bool* didcon )
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

dawn::Opt<dawn::EngineError> dawn::Engine::handle_instr( Ref<Node> const& node, Ref<Value>& retval, Int& push_count, Bool& didret, Bool* didbrk, Bool* didcon )
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

dawn::Opt<dawn::EngineError> dawn::Engine::handle_expr( Ref<Node> const& node, Ref<Value>& value )
{
    if ( auto nd = dynamic_cast<NothingNode*>(node.get()) )
        return handle_nothing_node( *nd, value );

    if ( auto nd = dynamic_cast<ValueNode*>(node.get()) )
        return handle_val_node( *nd, value );

    if ( auto nd = dynamic_cast<ArrayNode*>(node.get()) )
        return handle_array_node( *nd, value );

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

dawn::Opt<dawn::EngineError> dawn::Engine::handle_array_node( ArrayNode const& node, Ref<Value>& value )
{
    auto result = ArrayValue::make();

    if ( node.init_type == ArrayNode::InitType::SIZE )
    {
        Ref<Value> size_expr;
        if ( auto error = handle_expr( node.SIZE_size, size_expr ) )
            return error;

        Int size = size_expr->to_int();
        if ( size < 0 )
            return EngineError{ "Array size cannot be negative" };

        result->data.resize( size );
        for ( auto& val : result->data )
            val = node.SIZE_type->construct();
    }
    else
    {
        result->data.reserve( node.LIST_list.size() );
        for ( auto& expr : node.LIST_list )
        {
            Ref<Value> val;
            if ( auto error = handle_expr( expr, val ) )
                return error;
            result->data.push_back( std::move( val ) );
        }
    }

    value = result;

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

dawn::Opt<dawn::EngineError> dawn::Engine::handle_var_node( VariableNode const& node, Int& push_count )
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

dawn::Opt<dawn::EngineError> dawn::Engine::handle_func_node( FunctionNode const& node, Ref<Value>& retval )
{
    Array<Ref<Value>> args;
    for ( auto& arg : node.args )
    {
        Ref<Value> val;
        if ( auto error = handle_expr( arg, val ) )
            return error;
        args.emplace_back( std::move( val ) );
    }
    return call_func( node.name, args, retval );
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_return_node( ReturnNode const& node, Ref<Value>& retval, Bool& didret )
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

dawn::Opt<dawn::EngineError> dawn::Engine::handle_if_node( IfNode const& node, Ref<Value>& retval, Bool& didret, Bool* didbrk, Bool* didcon )
{
    Ref<Value> check_expr;
    if ( auto error = handle_expr( node.if_part.expr, check_expr ) )
        return error;
    if ( check_expr->to_bool() )
        return handle_scope( node.if_part.scope, retval, didret, didbrk, didcon );

    for ( auto& elif_part : node.elif_parts )
    {
        if ( auto error = handle_expr( elif_part.expr, check_expr ) )
            return error;
        if ( check_expr->to_bool() )
            return handle_scope( elif_part.scope, retval, didret, didbrk, didcon );
    }

    if ( node.else_part )
        return handle_scope( node.else_part->scope, retval, didret, didbrk, didcon );

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_switch_node( SwitchNode const& node, Ref<Value>& retval, Bool& didret, Bool* didbrk, Bool* didcon )
{
    assert( false && "not impl" );
    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_loop_node( LoopNode const& node, Ref<Value>& retval, Bool& didret )
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

dawn::Opt<dawn::EngineError> dawn::Engine::handle_while_node( WhileNode const& node, Ref<Value>& retval, Bool& didret )
{
    Bool didbrk = false, didcon = false;
    while ( true )
    {
        Ref<Value> check_expr;
        if ( auto error = handle_expr( node.expr, check_expr ) )
            return error;
        if ( !check_expr->to_bool() )
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

dawn::Opt<dawn::EngineError> dawn::Engine::handle_for_node( ForNode const& node, Ref<Value>& retval, Bool& didret )
{
    Ref<Value> loop_expr;
    if ( auto error = handle_expr( node.expr, loop_expr ) )
        return error;

    {
        EngineVar var;
        var.is_var = false;
        var.name = node.var_name;
        var.value = NothingValue::make();
        variables.push( var.name, var );
    }

    if ( auto value_rng = dynamic_cast<RangeValue const*>(loop_expr.get()) )
    {
        Bool didbrk = false, didcon = false;
        for ( auto i = value_rng->start_incl; i < value_rng->end_excl; ++i )
        {
            if ( didret || didbrk )
                break;
            if ( didcon )
                didcon = false;

            auto value = IntValue::make();
            value->value = i;
            variables.get( node.var_name )->value = std::move( value );

            if ( auto error = handle_scope( node.scope, retval, didret, &didbrk, &didcon ) )
                return error;
        }
    }
    else if ( auto value_str = dynamic_cast<StringValue const*>(loop_expr.get()) )
    {
        Bool didbrk = false, didcon = false;
        for ( Char c : value_str->value )
        {
            if ( didret || didbrk )
                break;
            if ( didcon )
                didcon = false;

            auto value = CharValue::make();
            value->value = c;
            variables.get( node.var_name )->value = std::move( value );

            if ( auto error = handle_scope( node.scope, retval, didret, &didbrk, &didcon ) )
                return error;
        }
    }
    else if ( auto value_arr = dynamic_cast<ArrayValue const*>(loop_expr.get()) )
    {
        Bool didbrk = false, didcon = false;
        for ( auto& value : value_arr->data )
        {
            if ( didret || didbrk )
                break;
            if ( didcon )
                didcon = false;

            variables.get( node.var_name )->value = value;

            if ( auto error = handle_scope( node.scope, retval, didret, &didbrk, &didcon ) )
                return error;
        }
    }
    else
        return EngineError{ "Can't for loop type: ", loop_expr->type() };

    variables.pop();

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
