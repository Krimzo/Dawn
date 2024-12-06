#pragma once

#include "values.h"


namespace dawn
{
struct NothingNode : Node
{
};

struct ValueNode : Node
{
    Ref<Value> value;
};

struct CastNode : Node
{
    Ref<Type> type;
    Ref<Node> expr;
};

struct SelfNode : Node
{
};

struct VariableNode : Node
{
    Variable var;
};

struct IdentifierNode : Node
{
    String name;
};

struct FunctionNode : Node
{
    String name;
    Array<Ref<Node>> args;
};

struct ReturnNode : Node
{
    Ref<Node> expr;
};

struct BreakNode : Node
{
};

struct ContinueNode : Node
{
};

struct IfNode : Node
{
    struct Part
    {
        Ref<Node> expr;
        Scope scope;
    };

    Part if_part;
    Array<Part> elif_parts;
    Opt<Part> else_part;
};

struct SwitchNode : Node
{
};

struct LoopNode : Node
{
    Scope scope;
};

struct WhileNode : Node
{
    Ref<Node> expr;
    Scope scope;
};

struct ForNode :Node
{
    String var_name;
    Ref<Node> expr;
    Scope scope;
};

struct StructNode : Node
{
    Ref<Type> type;
    Map<String, Ref<Node>> args;
};

struct ArrayNode : Node
{
    enum struct InitType
    {
        SIZE,
        LIST,
    };

    InitType init_type;
    Ref<Type> SIZE_type;
    Ref<Node> SIZE_size;
    Array<Ref<Node>> LIST_list;
};

struct UnaryNode : Node
{
    Ref<Node> right;
};

struct UnaryNodePlus : UnaryNode
{
};

struct UnaryNodeMinus : UnaryNode
{
};

struct UnaryNodeNot : UnaryNode
{
};

struct UnaryNodeRef : UnaryNode
{
};

struct UnaryNodeRange : UnaryNode
{
};

struct OperatorNode : Node
{
    Ref<Node> left;
    Ref<Node> right;
};

struct OperatorNodeLink : OperatorNode
{
};

struct OperatorNodeAccess : OperatorNode
{
};

struct OperatorNodeRange : OperatorNode
{
};

struct OperatorNodePow : OperatorNode
{
};

struct OperatorNodeMod : OperatorNode
{
};

struct OperatorNodeMul : OperatorNode
{
};

struct OperatorNodeDiv : OperatorNode
{
};

struct OperatorNodeAdd : OperatorNode
{
};

struct OperatorNodeSub : OperatorNode
{
};

struct OperatorNodeLess : OperatorNode
{
};

struct OperatorNodeGreat : OperatorNode
{
};

struct OperatorNodeLessEq : OperatorNode
{
};

struct OperatorNodeGreatEq : OperatorNode
{
};

struct OperatorNodeEq : OperatorNode
{
};

struct OperatorNodeNotEq : OperatorNode
{
};

struct OperatorNodeAnd : OperatorNode
{
};

struct OperatorNodeOr : OperatorNode
{
};

struct AssignNode : OperatorNode
{
};

struct AssignNodeAdd : AssignNode
{
};

struct AssignNodeSub : AssignNode
{
};

struct AssignNodeMul : AssignNode
{
};

struct AssignNodeDiv : AssignNode
{
};

struct AssignNodePow : AssignNode
{
};

struct AssignNodeMod : AssignNode
{
};
}
