#pragma once

#include "parser.h"
#include "stack.h"


namespace dawn
{
#define NAME_GEN(NAME) ID _##NAME = String( #NAME )

struct PredefinedNames
{
    NAME_GEN( bool );
    NAME_GEN( int );
    NAME_GEN( float );
    NAME_GEN( char );
    NAME_GEN( string );
    NAME_GEN( count );
    NAME_GEN( value );
};

struct Engine
{
    IDSystem id_system;
    PredefinedNames pre_names;
    Stack<ValueRef> variables;
    Map<Int, Function> functions;
    Map<Int, Enum> enums;
    Map<Int, Struct> structs;

    void load_standard();
    void load_mod( Module& module );
    void load_function( Function& entry );
    void load_enum( Enum& entry );
    void load_struct( Struct& entry );
    void load_variable( Variable& entry );

    void bind_func( StringRef const& name, Function::CppFunc cpp_func );
    void call_func( Int id, Array<Node>& args, ValueRef& retval );

    void add_var( Variable& var );
    void add_var( VariableKind kind, Int id, ValueRef const& value );
    ValueRef* get_var( Int id );

private:
    void handle_func( Function& func, Array<Node>& args, ValueRef& retval );
    void handle_scope( Scope& scope, ValueRef& retval, Bool& didret, Bool* didbrk, Bool* didcon );
    void handle_instr( Node& node, ValueRef& retval, Int& push_count, Bool& didret, Bool* didbrk, Bool* didcon );
    void handle_expr( Node& node, ValueRef& value );

    void handle_ref_node( RefNod& node, ValueRef& value );
    void handle_cast_node( CastNod& node, ValueRef& value );
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
