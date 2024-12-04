#pragma once

#include "parser.h"


namespace dawn
{
struct EngineError
{
    String msg;

    template<typename... Args>
    EngineError( Args const&... args )
    {
        StringStream stream;
        (stream << ... << args);
        msg = stream.str();
    }

    inline operator auto& () const
    {
        return msg;
    }
};

std::wostream& operator<<( std::wostream& stream, EngineError const& error );

struct EngineVar
{
    String name;
    Bool is_var = false;
    Ref<Type> type;
    Ref<Value> value;
};

template<typename T>
struct Stack
{
    void push( StringRef const& name, T const& val )
    {
        m_data.emplace_back( name, val );
    }

    void pop( Int n = 1 )
    {
        m_data.resize( m_data.size() - n );
    }

    T const& peek() const
    {
        return m_data.back();
    }

    T* get( StringRef const& name )
    {
        for ( Int i = (Int) m_data.size() - 1; i >= 0; i-- )
        {
            if ( m_data[i].first == name )
                return &m_data[i].second;
        }
        return nullptr;
    }

private:
    Array<Pair<String, T>> m_data;
};

struct Engine
{
    Opt<EngineError> load( Module const& module );
    Opt<EngineError> exec( String const& func_name, Array<Ref<Value>> const& args, Ref<Value>& retval, Bool allow_internal = false );
    Bool get_global( String const& var_name, Ref<Value>& value );

private:
    Stack<EngineVar> m_variables;
    Stack<Operator> m_operators;
    Stack<Function> m_functions;
    Stack<EnumType> m_enums;
    Stack<LayerType> m_layers;
    Stack<StructType> m_structs;

    Opt<EngineError> handle_expr( Ref<Node> const& node, Ref<Value>& value );
    Opt<EngineError> handle_instr( Ref<Node> const& node, Ref<Value>& retval, Bool& didret );
    Opt<EngineError> handle_func( Function const& func, Array<Ref<Value>> const& args, Ref<Value>& retval );

    Opt<EngineError> handle_val_node( ValueNode const& node, Ref<Value>& value );
    Opt<EngineError> handle_id_node( IdentifierNode const& node, Ref<Value>& value );
    Opt<EngineError> handle_func_call_node( FunctionCallNode const& node, Ref<Value>& value );
    Opt<EngineError> handle_print_node( PrintNode const& node, Ref<Value>& value );

    Opt<EngineError> handle_un_node( UnaryNode const& node, Ref<Value>& value );
    Opt<EngineError> handle_un_int( type_info const& node_type, Int right, Ref<Value>& value );
    Opt<EngineError> handle_un_float( type_info const& node_type, Float right, Ref<Value>& value );

    Opt<EngineError> handle_op_node( OperatorNode const& node, Ref<Value>& value );
    Opt<EngineError> handle_op_int( type_info const& node_type, Int left, Int right, Ref<Value>& value );
    Opt<EngineError> handle_op_float( type_info const& node_type, Float left, Float right, Ref<Value>& value );

    Opt<EngineError> handle_new_var_instr( NewVarNode const& node );
};
}
