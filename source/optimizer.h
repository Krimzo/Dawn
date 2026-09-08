#pragma once

#include "engine.h"

namespace dawn
{
struct Inlineable
{
    ID id;
    Value value{};
    Bool can_inline = false;
};

struct InlineDropper
{
    Vector<Inlineable>& inline_storage;
    const size_t initial_size;

    constexpr InlineDropper(Vector<Inlineable>& inline_storage)
        : inline_storage(inline_storage), initial_size(inline_storage.size())
    {
    }

    constexpr ~InlineDropper()
    {
        inline_storage.resize(initial_size);
    }
};

struct Optimizer
{
    Engine engine;
    Vector<Inlineable> inlines;

    void optimize(Module& module);
    void optimize_imports(StringSet& imports);
    void optimize_variables(Vector<Variable>& vars);
    void optimize_operators(Vector<Operator>& ops);
    void optimize_functions(Vector<Function>& funcs);
    void optimize_enums(Vector<Enum>& enums);
    void optimize_structs(Vector<Struct>& structs);

    void optimize_import(String& import);
    void optimize_variable(Variable& var);
    void optimize_operator(Operator& op);
    void optimize_function(Function& func);
    void optimize_enum(Enum& enu);
    void optimize_struct(Struct& struc);

    void optimize_instruction(Vector<Node>& scope);
    void optimize_expression(Node& node);
    void inline_variable(Variable& var, Vector<Node>& scope, Int& i);

    void optimize_expression_none(None& node, Node& out_node);
    void optimize_expression_scope(Scope& node, Node& out_node);
    void optimize_expression_variable(VariableNode& node, Node& out_node);
    void optimize_expression_return(ReturnNode& node, Node& out_node);
    void optimize_expression_break(BreakNode& node, Node& out_node);
    void optimize_expression_continue(ContinueNode& node, Node& out_node);
    void optimize_expression_throw(ThrowNode& node, Node& out_node);
    void optimize_expression_try(TryNode& node, Node& out_node);
    void optimize_expression_if(IfNode& node, Node& out_node);
    void optimize_expression_switch(SwitchNode& node, Node& out_node);
    void optimize_expression_loop(LoopNode& node, Node& out_node);
    void optimize_expression_while(WhileNode& node, Node& out_node);
    void optimize_expression_for(ForNode& node, Node& out_node);
    void optimize_expression_value(Value& node, Node& out_node);
    void optimize_expression_identifier(IdentifierNode& node, Node& out_node);
    void optimize_expression_call(CallNode& node, Node& out_node);
    void optimize_expression_index(IndexNode& node, Node& out_node);
    void optimize_expression_lambda(LambdaNode& node, Node& out_node);
    void optimize_expression_enum(EnumNode& node, Node& out_node);
    void optimize_expression_struct(StructNode& node, Node& out_node);
    void optimize_expression_array(ArrayNode& node, Node& out_node);
    void optimize_expression_access(AccessNode& node, Node& out_node);
    void optimize_expression_operator(OperatorNode& node, Node& out_node);
    void optimize_expression_assign(AssignNode& node, Node& out_node);
    void optimize_expression_cast(CastNode& node, Node& out_node);
};
} // namespace dawn
