#pragma once

#include "parser.h"
#include "stack.h"

namespace dawn
{
struct Engine
{
    using CastCFunc = Func<Value(Location, Engine&, Value const&)>;
    using MemberCFunc = Func<Value(Location, Engine&, Value const&)>;
    using FieldCFunc = Func<Value(Location, Engine&, Value const&)>;
    using MethodCFunc = Func<Value(Location, Engine&, Value const&, Value const*)>;

    friend struct Value;
    friend struct EnumValue;
    friend struct Optimizer;
    friend Value create_default_value(Engine* engine, ID typeid_, Location location);

    Stack stack;
    GlobalStorage<Enum> enums;
    GlobalStorage<Struct> structs;
    GlobalStorage<GlobalStorage<CastCFunc>> casts;
    GlobalStorage<GlobalStorage<MemberCFunc>> members;
    GlobalStorage<GlobalStorage<FunctionValue>> operators[(Int)OperatorType::_COUNT];

    Engine();

    void load_standard_operators();
    void load_standard_functions();
    void load_standard_members();
    void load_standard_casts();

    void load_module(Module const& module);
    void load_operator(Operator const& entry);
    void load_function(Function const& entry);
    void load_enum(Enum const& entry);
    void load_struct(Struct const& entry);
    void load_variable(Variable const& entry);

    void bind_operator(ID left_type_id, OperatorType op_type, ID right_type_id, Bool is_const, CFunction cfunc);
    void bind_cast(ID left_type_id, ID right_type_id, Bool is_const, CastCFunc const& cfunc);

    void bind_function(ID id, Bool is_ctime, CFunction cfunc);
    Value call_function(ID id, Value* args, Int arg_count);

    void add_variable(Location location, VarType const& type, ID id, Value const& value);
    Value* get_variable(ID id);

    void bind_field(ID type_id, ID id, FieldCFunc const& func);
    void bind_method(ID type_id, ID id, Bool is_const, Int expected_args, MethodCFunc const& func);

    Value create_default_value(ID type_id, Location location);

    void to_void(Value const& value);
    Bool to_bool(Value const& value);
    Int to_int(Value const& value);
    Float to_float(Value const& value);
    Char to_char(Value const& value);
    String to_string(Value const& value);
    RangeValue to_range(Value const& value);
    FunctionValue to_function(Value const& value);
    ArrayValue to_array(Value const& value);

    void handle_variable_node(VariableNode const& node);
    Value const& handle_identifier_node(IdentifierNode const& node);
    Value handle_call_node(CallNode const& node);
    Value handle_index_node(IndexNode const& node);
    void handle_return_node(ReturnNode const& node, Opt<Value>& retval);
    void handle_break_node(BreakNode const& node, Bool* didbrk);
    void handle_continue_node(ContinueNode const& node, Bool* didcon);
    void handle_throw_node(ThrowNode const& node);
    void handle_try_node(TryNode const& node, Opt<Value>& retval, Bool* didbrk, Bool* didcon);
    void handle_if_node(IfNode const& node, Opt<Value>& retval, Bool* didbrk, Bool* didcon);
    void handle_switch_node(SwitchNode const& node, Opt<Value>& retval, Bool* didbrk, Bool* didcon);
    void handle_loop_node(LoopNode const& node, Opt<Value>& retval);
    void handle_while_node(WhileNode const& node, Opt<Value>& retval);
    void handle_for_node(ForNode const& node, Opt<Value>& retval);
    Value const& handle_lambda_node(LambdaNode const& node);
    Value handle_enum_node(EnumNode const& node);
    Value handle_struct_node(StructNode const& node);
    Value handle_array_node(ArrayNode const& node);
    Value handle_access_node(AccessNode const& node);
    Value handle_operator_node(OperatorNode const& node);
    Value handle_assign_node(AssignNode const& node);
    Value handle_cast_node(CastNode const& node);

    void handle_scope(Scope const& scope, Opt<Value>& retval, Bool* didbrk, Bool* didcon);
    Value handle_operator(Location location, Value const& left, OperatorType op_type, Value const& right);
    Value handle_function(Location location, FunctionValue const& func, Value const* args, Int arg_count);
    Value handle_cast(Location location, Value const& left, ID cast_type);
    void handle_instruction(Node const& node, Opt<Value>& retval, Bool* didbrk, Bool* didcon);
    Value handle_expression(Node const& node);

  private:
    Set<uint64_t> m_ctime_ops[(Int)OperatorType::_COUNT] = {};
    Set<ID> m_ctime_funcs;
};
} // namespace dawn
