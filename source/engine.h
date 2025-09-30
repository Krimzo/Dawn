#pragma once

#include "parser.h"
#include "stack.h"


namespace dawn
{
struct Engine
{
    using CustomMemberFunc = Func<Value( Location const&, Value& )>;
    using CustomMethodFunc = Func<Value( Location const&, Value&, Value* )>;

    friend struct Value;
    friend struct EnumValue;
    friend struct Optimizer;

    Stack stack;
    GlobalStorage<Enum> enums;
    GlobalStorage<Struct> structs;
    GlobalStorage<MemberGenerator> member_generators[(Int) ValueType::_COUNT] = {};

    Engine();

    void load_mod( Module const& module );
    void load_function( Function const& entry );
    void load_enum( Enum const& entry );
    void load_struct( Struct const& entry );
    void load_variable( Variable const& entry );

    void bind_cfunc( Int id, Bool is_ctime, CFunction cfunc );
    Value call_func( Int id, Value* args, Int arg_count );

    void add_var( Location const& location, VarType const& type, Int id, Value const& value );
    Value* get_var( Int id );

    void bind_member( ValueType type, StringRef const& name, CustomMemberFunc const& func );
    void bind_method( ValueType type, StringRef const& name, Bool is_const, Int expected_args, CustomMethodFunc const& body );

    Set<Int> const& ctime_funcs() const;

private:
    Set<Int> m_ctime_funcs;

    void load_standard_functions();
    void load_standard_members();

    Value handle_func( Location const& location, FunctionValue const& func, Value* args, Int arg_count );
    void handle_scope( Scope const& scope, Opt<Value>& retval, Bool* didbrk, Bool* didcon );
    void handle_instr( Node const& node, Opt<Value>& retval, Bool* didbrk, Bool* didcon );
    Value handle_expr( Node const& node );

    Value handle_value_node( ValueNode const& node );
    void handle_var_node( VariableNode const& node );
    Value handle_id_node( IdentifierNode const& node );
    Value handle_call_node( CallNode const& node );
    Value handle_index_node( IndexNode const& node );
    void handle_return_node( ReturnNode const& node, Opt<Value>& retval );
    void handle_break_node( BreakNode const& node, Bool* didbrk );
    void handle_continue_node( ContinueNode const& node, Bool* didcon );
    void handle_throw_node( ThrowNode const& node );
    void handle_try_node( TryNode const& node, Opt<Value>& retval, Bool* didbrk, Bool* didcon );
    void handle_if_node( IfNode const& node, Opt<Value>& retval, Bool* didbrk, Bool* didcon );
    void handle_switch_node( SwitchNode const& node, Opt<Value>& retval, Bool* didbrk, Bool* didcon );
    void handle_loop_node( LoopNode const& node, Opt<Value>& retval );
    void handle_while_node( WhileNode const& node, Opt<Value>& retval );
    void handle_for_node( ForNode const& node, Opt<Value>& retval );
    Value handle_lambda_node( LambdaNode const& node );
    Value handle_enum_node( EnumNode const& node );
    Value handle_struct_node( StructNode const& node );
    Value handle_array_node( ArrayNode const& node );
    Value handle_un_node( UnaryNode const& node );
    Value handle_op_node( OperatorNode const& node );
    Value handle_ac_node( OperatorNode const& node );
    Value handle_as_node( AssignNode const& node );

    Value handle_ac_struct_node( Location const& location, Value const& self, Int right_id );
    Value handle_ac_type_node( Location const& location, Value const& self, Int right_id );

    Value create_default_value( Location const& location, Int typeid_ );
};
}
