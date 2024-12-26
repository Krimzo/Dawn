#pragma once

#include "parser.h"
#include "stack.h"


namespace dawn
{
using TypeMember = Func<ValueRef( ValueRef const& )>;

struct Engine
{
    friend struct ValueRef;

    ScopeStack stack;
    Map<Int, Enum> enums;
    Map<Int, Struct> structs;
    Map<Int, TypeMember> type_members[(Int) ValueType::_COUNT] = {};

    Engine();

    void load_mod( Module const& module );
    void load_function( Function const& entry );
    void load_enum( Enum const& entry );
    void load_struct( Struct const& entry );
    void load_variable( Variable const& entry );

    void bind_func( Int id, Function::CppFunc cpp_func );
    void call_func( Int id, ValueRef const* args, Int arg_count, ValueRef& retval );

    void add_obj( VariableKind kind, Int id, ValueRef const& value );
    ValueRef* get_obj( Int id );

private:
    void load_standard_functions();
    void load_nothing_members();
    void load_bool_members();
    void load_int_members();
    void load_float_members();
    void load_char_members();
    void load_string_members();
    void load_function_members();
    void load_enum_members();
    void load_array_members();
    void load_range_members();

    void handle_func( Function const& func, ValueRef const* args, Int arg_count, ValueRef& retval );
    void handle_scope( Scope const& scope, ValueRef& retval, Bool& didret, Bool* didbrk, Bool* didcon );
    void handle_instr( Node const& node, ValueRef& retval, Bool& didret, Bool* didbrk, Bool* didcon );
    void handle_expr( Node const& node, ValueRef& value );

    void handle_ref_node( RefNod const& node, ValueRef& value );
    void handle_var_node( VariableNod const& node );
    void handle_id_node( IdentifierNod const& node, ValueRef& value );
    void handle_call_node( CallNod const& node, ValueRef& retval );
    void handle_index_node( IndexNod const& node, ValueRef& retval );
    void handle_return_node( ReturnNod const& node, ValueRef& retval, Bool& didret );
    void handle_break_node( BreakNod const& node, Bool* didbrk );
    void handle_continue_node( ContinueNod const& node, Bool* didcon );
    void handle_throw_node( ThrowNod const& node );
    void handle_try_node( TryNod const& node, ValueRef& retval, Bool& didret, Bool* didbrk, Bool* didcon );
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

    void handle_ac_struct_node( ValueRef const& left, Int right, ValueRef& value );
    void handle_ac_type_node( ValueRef const& left, Int right, ValueRef& value );
};
}
