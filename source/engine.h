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

    void load_mod( Module& module );
    void load_function( Function& entry );
    void load_enum( Enum& entry );
    void load_struct( Struct& entry );
    void load_variable( Variable& entry );

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

    void handle_func( Function& func, ValueRef const* args, Int arg_count, ValueRef& retval );
    void handle_scope( Scope& scope, ValueRef& retval, Bool& didret, Bool* didbrk, Bool* didcon );
    void handle_instr( Node& node, ValueRef& retval, Bool& didret, Bool* didbrk, Bool* didcon );
    void handle_expr( Node& node, ValueRef& value );

    void handle_ref_node( RefNod& node, ValueRef& value );
    void handle_var_node( VariableNod& node );
    void handle_id_node( IdentifierNod& node, ValueRef& value );
    void handle_call_node( CallNod& node, ValueRef& retval );
    void handle_index_node( IndexNod& node, ValueRef& retval );
    void handle_return_node( ReturnNod& node, ValueRef& retval, Bool& didret );
    void handle_break_node( BreakNod& node, Bool* didbrk );
    void handle_continue_node( ContinueNod& node, Bool* didcon );
    void handle_throw_node( ThrowNod& node );
    void handle_try_node( TryNod& node, ValueRef& retval, Bool& didret, Bool* didbrk, Bool* didcon );
    void handle_if_node( IfNod& node, ValueRef& retval, Bool& didret, Bool* didbrk, Bool* didcon );
    void handle_switch_node( SwitchNod& node, ValueRef& retval, Bool& didret, Bool* didbrk, Bool* didcon );
    void handle_loop_node( LoopNod& node, ValueRef& retval, Bool& didret );
    void handle_while_node( WhileNod& node, ValueRef& retval, Bool& didret );
    void handle_for_node( ForNod& node, ValueRef& retval, Bool& didret );
    void handle_enum_node( EnumNod& node, ValueRef& value );
    void handle_struct_node( StructNod& node, ValueRef& value );
    void handle_array_node( ArrayNod& node, ValueRef& value );
    void handle_un_node( UnaryNod& node, ValueRef& value );
    void handle_op_node( OperatorNod& node, ValueRef& value );
    void handle_ac_node( OperatorNod& node, ValueRef& value );
    void handle_as_node( AssignNod& node, ValueRef& value );

    void handle_ac_struct_node( ValueRef const& left, Int right, ValueRef& value );
    void handle_ac_type_node( ValueRef const& left, Int right, ValueRef& value );
};
}
