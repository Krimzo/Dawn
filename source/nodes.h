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
    String type;
    Ref<Node> expr;
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
    struct Part
    {
        Array<Ref<Node>> exprs;
        Scope scope;
    };

    Ref<Node> main_expr;
    Array<Part> cases;
    Opt<Scope> def_scope;
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
    Variable var;
    Ref<Node> expr;
    Scope scope;
};

struct StructNode : Node
{
    String type;
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
    Ref<Node> SIZE_value_expr;
    Ref<Node> SIZE_size_expr;
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

struct UnaryNodeRange : UnaryNode
{
};

struct OperatorNode : Node
{
    Ref<Node> left;
    Ref<Node> right;
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

Ref<Node> make_value_literal( Ref<Value> const& value );

Ref<Node> make_bool_literal( Bool value );
Ref<Node> make_int_literal( Int value );
Ref<Node> make_float_literal( Float value );
Ref<Node> make_char_literal( Char value );
Ref<Node> make_string_literal( StringRef const& value );

Ref<Value> make_bool_value( Bool value );
Ref<Value> make_int_value( Int value );
Ref<Value> make_float_value( Float value );
Ref<Value> make_char_value( Char value );
Ref<Value> make_string_value( StringRef const& value );
}
