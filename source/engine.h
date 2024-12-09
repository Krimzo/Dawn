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
        stream << "Engine error: ";
        (stream << ... << args);
        msg = stream.str();
    }

    inline operator auto& () const
    {
        return msg;
    }
};

std::wostream& operator<<( std::wostream& stream, EngineError const& error );

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
        return m_data.back().second;
    }

    T* get( StringRef const& name )
    {
        for ( auto it = m_data.rbegin(); it != m_data.rend(); ++it )
        {
            if ( it->first == name )
                return &it->second;
        }
        return nullptr;
    }

private:
    List<Pair<String, T>> m_data;
};

struct Engine
{
    Stack<ValueBox> variables;
    Stack<Function> functions;
    Stack<Enum> enums;
    Stack<Struct> structs;

    void load_default_mods();
    Opt<EngineError> load_mod( Module const& module );

    void bind_func( String const& name, Function::CppFunc cpp_func );
    Opt<EngineError> call_func( String const& name, Array<Ref<Node>> const& args, ValueBox& retval );

    void add_var( String const& name, Bool is_var, RawValue const& value );
    void add_var( Variable const& var, ValueBox const& value );
    Opt<EngineError> add_var( Variable const& var );
    ValueBox* get_var( String const& name );

private:
    Opt<EngineError> handle_func( Function const& func, Array<Ref<Node>> const& args, ValueBox& retval );
    Opt<EngineError> handle_scope( Scope const& scope, ValueBox& retval, Bool& didret, Bool* didbrk, Bool* didcon );
    Opt<EngineError> handle_instr( Ref<Node> const& node, ValueBox& retval, Int& push_count, Bool& didret, Bool* didbrk, Bool* didcon );
    Opt<EngineError> handle_expr( Ref<Node> const& node, ValueBox& value );

    Opt<EngineError> handle_val_node( ValueNode const& node, ValueBox& value );
    Opt<EngineError> handle_array_node( ArrayNode const& node, ValueBox& value );
    Opt<EngineError> handle_struct_node( StructNode const& node, ValueBox& value );
    Opt<EngineError> handle_cast_node( CastNode const& node, ValueBox& value );
    Opt<EngineError> handle_var_node( VariableNode const& node, Int& push_count );
    Opt<EngineError> handle_id_node( IdentifierNode const& node, ValueBox& value );
    Opt<EngineError> handle_func_node( FunctionNode const& node, ValueBox& retval );
    Opt<EngineError> handle_return_node( ReturnNode const& node, ValueBox& retval, Bool& didret );
    Opt<EngineError> handle_break_node( BreakNode const& node, Bool* didbrk );
    Opt<EngineError> handle_continue_node( ContinueNode const& node, Bool* didcon );
    Opt<EngineError> handle_if_node( IfNode const& node, ValueBox& retval, Bool& didret, Bool* didbrk, Bool* didcon );
    Opt<EngineError> handle_switch_node( SwitchNode const& node, ValueBox& retval, Bool& didret, Bool* didbrk, Bool* didcon );
    Opt<EngineError> handle_loop_node( LoopNode const& node, ValueBox& retval, Bool& didret );
    Opt<EngineError> handle_while_node( WhileNode const& node, ValueBox& retval, Bool& didret );
    Opt<EngineError> handle_for_node( ForNode const& node, ValueBox& retval, Bool& didret );
    Opt<EngineError> handle_un_node( UnaryNode const& node, ValueBox& value );
    Opt<EngineError> handle_op_node( OperatorNode const& node, ValueBox& value );
    Opt<EngineError> handle_ac_node( OperatorNodeAccess const& node, ValueBox& value );
    Opt<EngineError> handle_as_node( AssignNode const& node, ValueBox& value );
};
}
