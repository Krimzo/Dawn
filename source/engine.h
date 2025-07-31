#pragma once

#include "parser.h"
#include "stack.h"


namespace dawn
{
using TypeMember = Func<Value( Value const& )>;

struct Engine
{
    friend struct Value;
    friend struct EnumVal;

    Stack stack;
    OMap<Int, Enum> enums;
    OMap<Int, Struct> structs;
    OMap<Int, TypeMember> type_members[(Int) ValueType::_COUNT] = {};

    Engine();

    void load_mod( Module const& module );
    void load_function( Function const& entry );
    void load_enum( Enum const& entry );
    void load_struct( Struct const& entry );
    void load_variable( Variable const& entry );

    void bind_func( Int id, Function::CppFunc cpp_func );
    Value call_func( Int id, Value* args, Int arg_count );

    void add_var( VariableKind kind, Int id, Value const& value );
    Value* get_var( Int id );

private:
    void load_standard_functions();
    void load_bool_members();
    void load_int_members();
    void load_float_members();
    void load_char_members();
    void load_string_members();
    void load_function_members();
    void load_enum_members();
    void load_array_members();
    void load_range_members();

    Value handle_func( Function const& func, Value* args, Int arg_count );
    void handle_scope( Scope const& scope, Opt<Value>& retval, Bool* didbrk, Bool* didcon );
    void handle_instr( Node const& node, Opt<Value>& retval, Bool* didbrk, Bool* didcon );
    Value handle_expr( Node const& node );

    Value handle_ref_node( RefNod const& node );
    void handle_var_node( VariableNod const& node );
    Value handle_id_node( IdentifierNod const& node );
    Value handle_call_node( CallNod const& node );
    Value handle_index_node( IndexNod const& node );
    void handle_return_node( ReturnNod const& node, Opt<Value>& retval );
    void handle_break_node( BreakNod const& node, Bool* didbrk );
    void handle_continue_node( ContinueNod const& node, Bool* didcon );
    void handle_throw_node( ThrowNod const& node );
    void handle_try_node( TryNod const& node, Opt<Value>& retval, Bool* didbrk, Bool* didcon );
    void handle_if_node( IfNod const& node, Opt<Value>& retval, Bool* didbrk, Bool* didcon );
    void handle_switch_node( SwitchNod const& node, Opt<Value>& retval, Bool* didbrk, Bool* didcon );
    void handle_loop_node( LoopNod const& node, Opt<Value>& retval );
    void handle_while_node( WhileNod const& node, Opt<Value>& retval );
    void handle_for_node( ForNod const& node, Opt<Value>& retval );
    Value handle_enum_node( EnumNod const& node );
    Value handle_struct_node( StructNod const& node );
    Value handle_array_node( ArrayNod const& node );
    Value handle_un_node( UnaryNod const& node );
    Value handle_op_node( OperatorNod const& node );
    Value handle_ac_node( OperatorNod const& node );
    Value handle_as_node( AssignNod const& node );

    Value handle_ac_struct_node( Value const& left, Int right );
    Value handle_ac_type_node( Value const& left, Int right );

    Value create_default_value( Int typeid_ );
};
}
