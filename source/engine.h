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

struct StackVal
{
    String name;
    Ref<Value> value;
};

struct Stack
{
    void push( StackVal const& val );
    void pop( Int n = 1 );
    StackVal const& peek() const;
    Ref<Value> get( StringRef const& name ) const;

private:
    Array<StackVal> m_data;
};

struct Engine
{
    Opt<EngineError> load( Module const& module );
    Opt<EngineError> exec( String const& func_name, Array<Ref<Value>> const& args, Ref<Value>& retval );
    Ref<Value> get_global( String const& var_name );

private:
    Map<String, Module> m_modules;
    Stack m_stack;

    Opt<EngineError> handle_expr( Ref<Node> const& node, Ref<Value>& value );
    Opt<EngineError> handle_instr( Ref<Node> const& node, Ref<Value>& retval );
    Opt<EngineError> handle_func( Function const& func, Array<Ref<Value>> const& args, Ref<Value>& retval );

    Opt<EngineError> handle_val_node( ValueNode const& node, Ref<Value>& value );
    Opt<EngineError> handle_id_node( IdentifierNode const& node, Ref<Value>& value );

    Opt<EngineError> handle_un_node( UnaryNode const& node, Ref<Value>& value );
    Opt<EngineError> handle_un_int( type_info const& node_type, Int right, Ref<Value>& value );
    Opt<EngineError> handle_un_float( type_info const& node_type, Float right, Ref<Value>& value );

    Opt<EngineError> handle_op_node( OperatorNode const& node, Ref<Value>& value );
    Opt<EngineError> handle_op_int( type_info const& node_type, Int left, Int right, Ref<Value>& value );
    Opt<EngineError> handle_op_float( type_info const& node_type, Float left, Float right, Ref<Value>& value );
};
}
