#pragma once

#include "parser.h"
#include "stack.h"


namespace dawn
{
#define PRE_OP_GEN(NAME, OP) ID __##NAME = String( OP )
#define PRE_NAME_GEN(NAME) ID _##NAME = String( #NAME )

struct Predefines
{
    PRE_OP_GEN( add, op_add );
    PRE_OP_GEN( sub, op_sub );
    PRE_OP_GEN( mul, op_mul );
    PRE_OP_GEN( div, op_div );
    PRE_OP_GEN( pow, op_pow );
    PRE_OP_GEN( mod, op_mod );
    PRE_OP_GEN( cmpr, op_cmpr );

    PRE_NAME_GEN( bool );
    PRE_NAME_GEN( int );
    PRE_NAME_GEN( float );
    PRE_NAME_GEN( char );
    PRE_NAME_GEN( string );

    PRE_NAME_GEN( to_bool );
    PRE_NAME_GEN( to_int );
    PRE_NAME_GEN( to_float );
    PRE_NAME_GEN( to_char );
    PRE_NAME_GEN( to_string );

    PRE_NAME_GEN( count );
    PRE_NAME_GEN( value );
};

struct Engine
{
    friend struct ValueRef;

    IDSystem id_system;
    Predefines predefines;
    Stack<ValueRef> stack;
    Map<Int, Enum> enums;
    Map<Int, Struct> structs;

    void load_standard();
    void load_mod( Module& module );
    void load_function( Function& entry );
    void load_enum( Enum& entry );
    void load_struct( Struct& entry );
    void load_variable( Variable& entry );

    void bind_func( StringRef const& name, Function::CppFunc cpp_func );
    void call_func( Int id, Array<ValueRef>& args, ValueRef& retval );

    void add_var( VariableKind kind, Int id, ValueRef const& value );
    ValueRef* get_var( Int id );

private:
    void handle_func( Function& func, Array<ValueRef>& args, ValueRef& retval );
    void handle_scope( Scope& scope, ValueRef& retval, Bool& didret, Bool* didbrk, Bool* didcon );
    void handle_instr( Node& node, ValueRef& retval, Int& push_count, Bool& didret, Bool* didbrk, Bool* didcon );
    void handle_expr( Node& node, ValueRef& value );

    void handle_ref_node( RefNod& node, ValueRef& value );
    void handle_var_node( VariableNod& node, Int& push_count );
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

    void handle_ac_string_node( ValueRef const& left, Node& right, ValueRef& value );
    void handle_ac_enum_node( ValueRef const& left, Node& right, ValueRef& value );
    void handle_ac_struct_node( ValueRef const& left, Node& right, ValueRef& value );
    void handle_ac_array_node( ValueRef const& left, Node& right, ValueRef& value );
};

struct PopHandler
{
    Engine& engine;
    Int count = 0;

    PopHandler( Engine& engine );
    ~PopHandler() noexcept;
};
}
