#pragma once

#include "stack.h"


namespace dawn
{
struct None
{
};

struct ValueNode
{
    Value value;
};

struct VariableNode
{
    Variable var;
};

struct IdentifierNode
{
    Int id = 0;
};

struct CallNode
{
    NodeRef left_expr;
    Vector<Node> args;
};

struct IndexNode
{
    NodeRef left_expr;
    NodeRef expr;
};

struct ReturnNode
{
    NodeRef expr;
};

struct BreakNode
{
};

struct ContinueNode
{
};

struct ThrowNode
{
    NodeRef expr;
};

struct TryNode
{
    Scope try_scope;
    Int catch_id = 0;
    Scope catch_scope;
};

struct IfNode
{
    template<typename T>
    struct Part // had to cheat the compiler with a template :)
    {
        T expr;
        Scope scope;
    };

    Vector<Part<Node>> parts;
};

struct SwitchNode
{
    struct Part
    {
        Vector<Node> exprs;
        Scope scope;
    };

    NodeRef main_expr;
    Vector<Part> cases;
    Opt<Scope> def_scope;
};

struct LoopNode
{
    Scope scope;
};

struct WhileNode
{
    NodeRef expr;
    Scope scope;
};

struct ForNode
{
    Variable var;
    NodeRef expr;
    Scope scope;
};

struct EnumNode
{
    Int type_id = 0;
    Int key_id = 0;
};

struct StructNode
{
    Int type_id = 0;
    OMap<Int, Node> args;
};

struct ArrayNode
{
    struct ListInit
    {
        Vector<Node> elements;
    };

    struct SizedInit
    {
        Int type_id = 0;
        NodeRef size_expr;
    };

    Variant<ListInit, SizedInit> init;
};

struct UnaryNode
{
    UnaryType type;
    NodeRef right;
};

struct OperatorNode
{
    OperatorType type;
    Vector<Node> sides;
};

struct AssignNode
{
    AssignType type;
    Vector<Node> sides;
};

struct Node : Variant <
    None,
    Scope,
    VariableNode,
    ReturnNode,
    BreakNode,
    ContinueNode,
    ThrowNode,
    TryNode,
    IfNode,
    SwitchNode,
    LoopNode,
    WhileNode,
    ForNode,
    ValueNode,
    IdentifierNode,
    CallNode,
    IndexNode,
    EnumNode,
    StructNode,
    ArrayNode,
    UnaryNode,
    OperatorNode,
    AssignNode >
{
    inline NodeType type() const
    {
        return (NodeType) index();
    }
};

Node make_nothing_node();
Node make_bool_node( Bool value );
Node make_int_node( Int value );
Node make_float_node( Float value );
Node make_char_node( Char value );
Node make_string_node( StringRef const& value );
Node make_value_node( Value const& value );
}
