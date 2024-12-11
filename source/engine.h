#pragma once

#include "parser.h"


namespace dawn
{
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
    Array<Pair<String, T>> m_data;
};

struct Engine
{
    Stack<ValueBox> variables;
    Map<String, Function> functions;
    Map<String, Enum> enums;
    Map<String, Struct> structs;

    void load_default_mods();
    void load_mod( Module const& module );

    void bind_func( String const& name, Function::CppFunc cpp_func );
    void call_func( String const& name, Array<Ref<Node>> const& args, ValueBox& retval );

    void add_var( Variable const& var );
    void add_var( Variable const& var, ValueBox const& value );
    ValueBox* get_var( String const& name );

private:
    void handle_func( Function const& func, Array<Ref<Node>> const& args, ValueBox& retval );
    void handle_scope( Scope const& scope, ValueBox& retval, Bool& didret, Bool* didbrk, Bool* didcon );
    void handle_instr( Ref<Node> const& node, ValueBox& retval, Int& push_count, Bool& didret, Bool* didbrk, Bool* didcon );
    void handle_expr( Ref<Node> const& node, ValueBox& value );

    void handle_val_node( ValueNode const& node, ValueBox& value );
    void handle_box_node( BoxNode const& node, ValueBox& value );
    void handle_cast_node( CastNode const& node, ValueBox& value );
    void handle_var_node( VariableNode const& node, Int& push_count );
    void handle_id_node( IdentifierNode const& node, ValueBox& value );
    void handle_func_node( FunctionNode const& node, ValueBox& retval );
    void handle_return_node( ReturnNode const& node, ValueBox& retval, Bool& didret );
    void handle_break_node( BreakNode const& node, Bool* didbrk );
    void handle_continue_node( ContinueNode const& node, Bool* didcon );
    void handle_if_node( IfNode const& node, ValueBox& retval, Bool& didret, Bool* didbrk, Bool* didcon );
    void handle_switch_node( SwitchNode const& node, ValueBox& retval, Bool& didret, Bool* didbrk, Bool* didcon );
    void handle_loop_node( LoopNode const& node, ValueBox& retval, Bool& didret );
    void handle_while_node( WhileNode const& node, ValueBox& retval, Bool& didret );
    void handle_for_node( ForNode const& node, ValueBox& retval, Bool& didret );
    void handle_enum_node( EnumNode const& node, ValueBox& value );
    void handle_struct_node( StructNode const& node, ValueBox& value );
    void handle_array_node( ArrayNode const& node, ValueBox& value );
    void handle_un_node( UnaryNode const& node, ValueBox& value );
    void handle_op_node( OperatorNode const& node, ValueBox& value );
    void handle_ac_node( OperatorNode const& node, ValueBox& value );
    void handle_as_node( AssignNode const& node, ValueBox& value );

    void handle_ac_string_node( ValueBox const& left, Ref<Node> const& right, ValueBox& value );
    void handle_ac_enum_node( ValueBox const& left, Ref<Node> const& right, ValueBox& value );
    void handle_ac_struct_node( ValueBox const& left, Ref<Node> const& right, ValueBox& value );
    void handle_ac_array_node( ValueBox const& left, Ref<Node> const& right, ValueBox& value );
};
}
