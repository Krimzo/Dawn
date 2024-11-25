#include "engine.h"


std::wostream& dawn::operator<<( std::wostream& stream, EngineError const& error )
{
    stream << error.msg;
    return stream;
}

void dawn::Stack::push( StackVal const& val )
{
    m_data.emplace_back( val );
}

void dawn::Stack::pop( Int n )
{
    m_data.resize( m_data.size() - n );
}

dawn::StackVal const& dawn::Stack::peek() const
{
    return m_data.back();
}

dawn::Ref<dawn::Value> dawn::Stack::get( StringRef const& name ) const
{
    for ( Int i = m_data.size() - 1; i >= 0; i-- )
    {
        if ( m_data[i].name == name )
            return m_data[i].value;
    }
    return {};
}

dawn::Opt<dawn::EngineError> dawn::Engine::load( Module const& module )
{
    if ( m_modules.contains( module.name ) )
        return EngineError{ L"module [", module.name, L"] already loaded" };

    Set<String> keys;
    keys.insert( std::views::keys( module.space_public.variables ).begin(), std::views::keys( module.space_public.variables ).end() );
    keys.insert( std::views::keys( module.space_public.functions ).begin(), std::views::keys( module.space_public.functions ).end() );
    keys.insert( std::views::keys( module.space_public.enums ).begin(), std::views::keys( module.space_public.enums ).end() );
    keys.insert( std::views::keys( module.space_public.layers ).begin(), std::views::keys( module.space_public.layers ).end() );
    keys.insert( std::views::keys( module.space_public.structs ).begin(), std::views::keys( module.space_public.structs ).end() );

    for ( auto& key : keys )
    {
        for ( auto& [_, mod] : m_modules )
        {
            if ( mod.contains_id( key ) )
                return EngineError{ L"[", key, L"] already exists" };
        }
    }

    m_modules[module.name] = module;
    for ( auto& [name, node] : module.space_public.variables )
    {
        Ref<Value> value;
        if ( auto error = handle_expr( node.expr, value ) )
            return error;
        m_stack.push( { name, value } );
    }

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::exec( String const& func_name, Array<Ref<Value>> const& args, Ref<Value>& retval )
{
    Function* func = nullptr;
    for ( auto& [_, mod] : m_modules )
    {
        auto& functions = mod.space_public.functions;
        if ( functions.contains( func_name ) )
        {
            func = &functions.at( func_name );
            break;
        }
    }

    if ( !func )
        return EngineError{ L"function doesn't exist" };

    if ( auto error = handle_func( *func, args, retval ) )
        return error;

    return std::nullopt;
}

dawn::Ref<dawn::Value> dawn::Engine::get_global( String const& var_name )
{
    return m_stack.get( var_name );
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

    return EngineError{ "Unknown expr node type: ", typeid(*node).name() };
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_instr( Ref<Node> const& node, Ref<Value>& retval )
{
    assert( false && "not impl" );
    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_func( Function const& func, Array<Ref<Value>> const& args, Ref<Value>& retval )
{
    if ( func.args.size() != args.size() )
        return EngineError{ "invalid argument count" };

    for ( Int i = 0; i < (Int) args.size(); i++ )
        m_stack.push( { func.args[i].name, args[i] } );

    for ( auto& instr : func.body.instr )
    {
        if ( auto error = handle_instr( instr, retval ) )
            return error;
    }

    m_stack.pop( (Int) args.size() );

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_val_node( ValueNode const& node, Ref<Value>& value )
{
    value = node.value;
    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_id_node( IdentifierNode const& node, Ref<Value>& value )
{
    value = m_stack.get( node.name );
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
    Ref<Value> left;
    if ( auto error = handle_expr( node.left, left ) )
        return error;

    Opt<Int> left_int;
    Opt<Float> left_float;

    if ( auto val = dynamic_cast<IntValue*>(left.get()) )
        left_int = val->value;
    else if ( auto val = dynamic_cast<FloatValue*>(left.get()) )
        left_float = val->value;
    else return EngineError{ "operators support only ints and floats" };

    Ref<Value> right;
    if ( auto error = handle_expr( node.right, right ) )
        return error;

    Opt<Int> right_int;
    Opt<Float> right_float;

    if ( auto val = dynamic_cast<IntValue*>(right.get()) )
        right_int = val->value;
    else if ( auto val = dynamic_cast<FloatValue*>(right.get()) )
        right_float = val->value;
    else return EngineError{ "operators support only ints and floats" };

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
