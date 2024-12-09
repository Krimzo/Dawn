#include "engine.h"


std::wostream& dawn::operator<<( std::wostream& stream, EngineError const& error )
{
    stream << "Error: " << error.msg;
    return stream;
}

dawn::EngineVariableLet::EngineVariableLet( Ref<Value> const& value )
{
    if ( auto ref_val = dynamic_cast<RefValue const*>(value.get()) )
        m_value = ref_val->eng_var->get_value();
    else
        m_value = value;
}

dawn::Ref<dawn::Value> const& dawn::EngineVariableLet::get_value() const
{
    return m_value;
}

dawn::Opt<dawn::EngineError> dawn::EngineVariableLet::set_value( Ref<Value> const& value )
{
    return EngineError{ "Cannot set value of a let variable" };
}

dawn::EngineVariableVar::EngineVariableVar( Ref<Value> const& value )
{
    set_value( value );
}

dawn::Ref<dawn::Value> const& dawn::EngineVariableVar::get_value() const
{
    return m_value;
}

dawn::Opt<dawn::EngineError> dawn::EngineVariableVar::set_value( Ref<Value> const& value )
{
    if ( auto ref_val = dynamic_cast<RefValue const*>(value.get()) )
        m_value = ref_val->eng_var->get_value();
    else
        m_value = value;

    return std::nullopt;
}

dawn::EngineVariableRef::EngineVariableRef( Ref<EngineVariable> const& ref_var )
{
    m_ref_var = ref_var;
}

dawn::Ref<dawn::Value> const& dawn::EngineVariableRef::get_value() const
{
    return m_ref_var->get_value();
}

dawn::Opt<dawn::EngineError> dawn::EngineVariableRef::set_value( Ref<Value> const& value )
{
    return m_ref_var->set_value( value );
}

dawn::Opt<dawn::EngineError> dawn::Engine::load_mod( Module const& module )
{
    auto helper_func = [this]( auto& out_coll, auto const& in_coll )
    {
        for ( auto& entry : in_coll )
            out_coll.push( entry.name, entry );
    };

    helper_func( enums, module.enums );
    helper_func( layers, module.layers );
    helper_func( structs, module.structs );
    helper_func( functions, module.functions );

    for ( auto& entry : module.variables )
    {
        if ( auto error = add_var( entry ) )
            return error;
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

dawn::Opt<dawn::EngineError> dawn::Engine::call_func( String const& name, Array<Ref<Node>> const& args, Ref<Value>& retval )
{
    auto* func = functions.get( name );
    if ( !func )
        return EngineError{ L"function [", name, L"] doesn't exist" };

    if ( auto error = handle_func( *func, args, retval ) )
        return error;

    return std::nullopt;
}

void dawn::Engine::add_var( String const& name, Bool is_var, Ref<Value> const& value )
{
    Ref<EngineVariable> eng_var;
    if ( !is_var )
    {
        eng_var = std::make_shared<EngineVariableLet>( value );
    }
    else
    {
        eng_var = std::make_shared<EngineVariableVar>( value );
    }
    variables.push( name, eng_var );
}

dawn::Opt<dawn::EngineError> dawn::Engine::add_var( Variable const& var )
{
    Ref<Value> value;
    if ( auto error = handle_expr( var.expr, value ) )
        return error;

    Ref<EngineVariable> eng_var;
    if ( var.kind == Variable::Kind::LET )
    {
        eng_var = std::make_shared<EngineVariableLet>( value );
    }
    else if ( var.kind == Variable::Kind::VAR )
    {
        eng_var = std::make_shared<EngineVariableVar>( value );
    }
    else
    {
        auto ref_val = dynamic_cast<RefValue const*>(value.get());
        if ( !ref_val )
            return EngineError{ L"variable [", var.name, L"] can't reference [", value->type(), L"]" };

        eng_var = std::make_shared<EngineVariableRef>( ref_val->eng_var );
    }

    variables.push( var.name, eng_var );

    return std::nullopt;
}

dawn::EngineVariable* dawn::Engine::get_var( String const& name )
{
    auto* ptr = variables.get( name );
    return ptr ? ptr->get() : nullptr;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_func( Function const& func, Array<Ref<Node>> const& args, Ref<Value>& retval )
{
    retval = NothingValue::make();

    if ( func.body.index() == 1 )
    {
        Array<Ref<Value>> arg_values;
        for ( auto& arg : args )
        {
            Ref<Value> value;
            if ( auto error = handle_expr( arg, value ) )
                return error;
            arg_values.push_back( std::move( value ) );
        }

        try
        {
            retval = std::get<Function::CppFunc>( func.body )(arg_values);
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
        Ref<Value> size_val;
        if ( auto error = handle_expr( node.SIZE_size_expr, size_val ) )
            return error;

        Int size = size_val->to_int();
        if ( size < 0 )
            return EngineError{ "Array size cannot be negative" };

        Ref<Value> value_val;
        if ( auto error = handle_expr( node.SIZE_value_expr, value_val ) )
            return error;

        result->data.resize( size );
        for ( auto& val : result->data )
            val = value_val->clone();
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
        if ( node.type == tp_bool )
        {
            value = make_bool_value( expr->to_bool() );
        }
        else if ( node.type == tp_int )
        {
            value = make_int_value( expr->to_int() );
        }
        else if ( node.type == tp_float )
        {
            value = make_float_value( expr->to_float() );
        }
        else if ( node.type == tp_char )
        {
            value = make_char_value( expr->to_char() );
        }
        else if ( node.type == tp_string )
        {
            value = make_string_value( expr->to_string() );
        }
        else
            return EngineError{ "Unknown cast type: ", node.type };
    }
    catch ( String& msg )
    {
        return EngineError{ msg };
    }

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_var_node( VariableNode const& node, Int& push_count )
{
    if ( auto error = add_var( node.var ) )
        return error;

    ++push_count;

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_id_node( IdentifierNode const& node, Ref<Value>& value )
{
    auto* ptr = variables.get( node.name );
    if ( !ptr )
        return EngineError{ L"variable [", node.name, L"] doesn't exist" };

    auto result = RefValue::make();
    result->eng_var = *ptr;

    value = result;

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_func_node( FunctionNode const& node, Ref<Value>& retval )
{
    auto* func = functions.get( node.name );
    if ( !func )
        return EngineError{ L"function [", node.name, L"] doesn't exist" };

    return handle_func( *func, node.args, retval );
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
    Ref<Value> check_expr;
    if ( auto error = handle_expr( node.main_expr, check_expr ) )
        return error;

    for ( auto& case_part : node.cases )
    {
        for ( auto& expr : case_part.exprs )
        {
            Ref<Value> val;
            if ( auto error = handle_expr( expr, val ) )
                return error;

            if ( (*check_expr == *val)->to_bool() )
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
    ForNode node_cpy = node;

    Ref<Value> loop_expr;
    if ( auto error = handle_expr( node_cpy.expr, loop_expr ) )
        return error;

    if ( auto ref_val = dynamic_cast<RefValue const*>(loop_expr.get()) )
        loop_expr = ref_val->eng_var->get_value();

    if ( auto value_rng = dynamic_cast<RangeValue const*>(loop_expr.get()) )
    {
        Bool didbrk = false, didcon = false;
        for ( auto i = value_rng->start_incl; i < value_rng->end_excl; ++i )
        {
            if ( didret || didbrk )
                break;
            if ( didcon )
                didcon = false;

            node_cpy.var.expr = make_int_literal( i );
            if ( auto error = add_var( node_cpy.var ) )
                return error;

            if ( auto error = handle_scope( node_cpy.scope, retval, didret, &didbrk, &didcon ) )
                return error;

            variables.pop();
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

            node_cpy.var.expr = make_char_literal( c );
            if ( auto error = add_var( node_cpy.var ) )
                return error;

            if ( auto error = handle_scope( node_cpy.scope, retval, didret, &didbrk, &didcon ) )
                return error;

            variables.pop();
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

            node_cpy.var.expr = make_value_literal( value );
            if ( auto error = add_var( node_cpy.var ) )
                return error;

            if ( auto error = handle_scope( node_cpy.scope, retval, didret, &didbrk, &didcon ) )
                return error;

            variables.pop();
        }
    }
    else
        return EngineError{ "Can't for loop type: ", loop_expr->type() };

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
    Ref<Value> left;
    if ( auto error = handle_expr( node.left, left ) )
        return error;

    auto left_ref = dynamic_cast<RefValue const*>(left.get());
    if ( !left_ref )
        return EngineError{ "Can't assign to [", left->type(), "]" };

    Ref<Value> right;
    if ( auto error = handle_expr( node.right, right ) )
        return error;

    auto& var = left_ref->eng_var;

    if ( typeid(node) == typeid(AssignNodeAdd) )
    {
        if ( auto error = var->set_value( *var->get_value() + (*right) ) )
            return error;
    }
    else if ( typeid(node) == typeid(AssignNodeSub) )
    {
        if ( auto error = var->set_value( *var->get_value() - (*right) ) )
            return error;
    }
    else if ( typeid(node) == typeid(AssignNodeMul) )
    {
        if ( auto error = var->set_value( *var->get_value() * (*right) ) )
            return error;
    }
    else if ( typeid(node) == typeid(AssignNodeDiv) )
    {
        if ( auto error = var->set_value( *var->get_value() / (*right) ) )
            return error;
    }
    else if ( typeid(node) == typeid(AssignNodePow) )
    {
        if ( auto error = var->set_value( *var->get_value() ^ (*right) ) )
            return error;
    }
    else if ( typeid(node) == typeid(AssignNodeMod) )
    {
        if ( auto error = var->set_value( *var->get_value() % (*right) ) )
            return error;
    }
    else
    {
        if ( auto error = var->set_value( std::move( right ) ) )
            return error;
    }

    value = left;

    return std::nullopt;
}
