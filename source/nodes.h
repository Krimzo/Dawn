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

struct SelfNode : Node
{
};

struct IdentifierNode : Node
{
    String name;
};

struct CastNode : Node
{
    Ref<Type> type;
    Ref<Node> expr;
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
        SIZE_VAL,
        LIST,
    };

    Ref<Type> type;
    InitType init_type;
    Ref<Node> size;
    Ref<Node> _val;
    Array<Ref<Node>> _list;
};

struct VariableNode : Node
{
    Variable var;
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

/* should rework */ struct PrintNode : Node
{
    Array<Ref<Node>> args;
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
