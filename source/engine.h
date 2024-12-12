#pragma once

#include "parser.h"


namespace dawn
{
template<typename T>
struct Stack
{
    Stack( Int reserve = 512 )
    {
        m_data.reserve( reserve );
    }

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
    Stack<ValueRef> variables;
    Map<String, Function> functions;
    Map<String, Enum> enums;
    Map<String, Struct> structs;

    void load_default_mods();
    void load_mod( Module const& module );

    void bind_func( String const& name, Function::CppFunc cpp_func );
    void call_func( String const& name, Array<Node> const& args, ValueRef& retval );

    void add_var( Variable const& var );
    void add_var( Variable const& var, ValueRef const& value );
    ValueRef* get_var( String const& name );

private:
    void handle_func( Function const& func, Array<Node> const& args, ValueRef& retval );
    void handle_scope( Scope const& scope, ValueRef& retval, Bool& didret, Bool* didbrk, Bool* didcon );
    void handle_instr( Node const& node, ValueRef& retval, Int& push_count, Bool& didret, Bool* didbrk, Bool* didcon );
    void handle_expr( Node const& node, ValueRef& value );

    void handle_val_node( ValueNod const& node, ValueRef& value );
    void handle_box_node( BoxNod const& node, ValueRef& value );
    void handle_cast_node( CastNod const& node, ValueRef& value );
    void handle_var_node( VariableNod const& node, Int& push_count );
    void handle_id_node( IdentifierNod const& node, ValueRef& value );
    void handle_func_node( FunctionNod const& node, ValueRef& retval );
    void handle_return_node( ReturnNod const& node, ValueRef& retval, Bool& didret );
    void handle_break_node( BreakNod const& node, Bool* didbrk );
    void handle_continue_node( ContinueNod const& node, Bool* didcon );
    void handle_if_node( IfNod const& node, ValueRef& retval, Bool& didret, Bool* didbrk, Bool* didcon );
    void handle_switch_node( SwitchNod const& node, ValueRef& retval, Bool& didret, Bool* didbrk, Bool* didcon );
    void handle_loop_node( LoopNod const& node, ValueRef& retval, Bool& didret );
    void handle_while_node( WhileNod const& node, ValueRef& retval, Bool& didret );
    void handle_for_node( ForNod const& node, ValueRef& retval, Bool& didret );
    void handle_enum_node( EnumNod const& node, ValueRef& value );
    void handle_struct_node( StructNod const& node, ValueRef& value );
    void handle_array_node( ArrayNod const& node, ValueRef& value );
    void handle_un_node( UnaryNod const& node, ValueRef& value );
    void handle_op_node( OperatorNod const& node, ValueRef& value );
    void handle_ac_node( OperatorNod const& node, ValueRef& value );
    void handle_as_node( AssignNod const& node, ValueRef& value );

    void handle_ac_string_node( ValueRef const& left, Node const& right, ValueRef& value );
    void handle_ac_enum_node( ValueRef const& left, Node const& right, ValueRef& value );
    void handle_ac_struct_node( ValueRef const& left, Node const& right, ValueRef& value );
    void handle_ac_array_node( ValueRef const& left, Node const& right, ValueRef& value );
};
}
