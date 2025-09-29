#pragma once

#include "stack.h"


namespace dawn
{
struct LocationHolder
{
    Location location;

    explicit constexpr LocationHolder( Location const& location )
        : location( location )
    {
    }
};

struct None
{
};

struct ValueNode : LocationHolder
{
    Value value;

    constexpr ValueNode( Location const& location )
        : LocationHolder( location )
    {
    }
};

struct VariableNode : LocationHolder
{
    Variable var;

    constexpr VariableNode( Location const& location )
        : LocationHolder( location )
    {
    }
};

struct IdentifierNode : LocationHolder
{
    Int id = 0;

    constexpr IdentifierNode( Location const& location )
        : LocationHolder( location )
    {
    }
};

struct CallNode : LocationHolder
{
    NodeRef left_expr;
    Vector<Node> args;

    constexpr CallNode( Location const& location )
        : LocationHolder( location )
    {
    }
};

struct IndexNode : LocationHolder
{
    NodeRef left_expr;
    NodeRef expr;

    constexpr IndexNode( Location const& location )
        : LocationHolder( location )
    {
    }
};

struct ReturnNode : LocationHolder
{
    NodeRef expr;

    constexpr ReturnNode( Location const& location )
        : LocationHolder( location )
    {
    }
};

struct BreakNode : LocationHolder
{
    constexpr BreakNode( Location const& location )
        : LocationHolder( location )
    {
    }
};

struct ContinueNode : LocationHolder
{
    constexpr ContinueNode( Location const& location )
        : LocationHolder( location )
    {
    }
};

struct ThrowNode : LocationHolder
{
    NodeRef expr;

    constexpr ThrowNode( Location const& location )
        : LocationHolder( location )
    {
    }
};

struct TryNode : LocationHolder
{
    Scope try_scope;
    Int catch_id = 0;
    Scope catch_scope;

    constexpr TryNode( Location const& location )
        : LocationHolder( location )
    {
    }
};

struct IfNode : LocationHolder
{
    template<typename T> // Template is required because Node does not exist at this stage.
    struct Part
    {
        T expr;
        Scope scope;
    };

    Vector<Part<Node>> parts;

    constexpr IfNode( Location const& location )
        : LocationHolder( location )
    {
    }
};

struct SwitchNode : LocationHolder
{
    struct Case
    {
        Vector<Node> exprs;
        Scope scope;
    };

    NodeRef main_expr;
    Vector<Case> cases;
    Opt<Scope> def_scope;

    constexpr SwitchNode( Location const& location )
        : LocationHolder( location )
    {
    }
};

struct LoopNode : LocationHolder
{
    Scope scope;

    constexpr LoopNode( Location const& location )
        : LocationHolder( location )
    {
    }
};

struct WhileNode : LocationHolder
{
    NodeRef expr;
    Scope scope;

    constexpr WhileNode( Location const& location )
        : LocationHolder( location )
    {
    }
};

struct ForNode : LocationHolder
{
    Int var_id = 0;
    NodeRef expr;
    Scope scope;

    constexpr ForNode( Location const& location )
        : LocationHolder( location )
    {
    }
};

struct LambdaNode : LocationHolder
{
    Value func_value;

    constexpr LambdaNode( Location const& location )
        : LocationHolder( location )
    {
    }
};

struct EnumNode : LocationHolder
{
    Int type_id = 0;
    Int key_id = 0;

    constexpr EnumNode( Location const& location )
        : LocationHolder( location )
    {
    }
};

struct StructNode : LocationHolder
{
    struct NamedInit
    {
        Map<Int, Node> args;
    };

    struct ListInit
    {
        Vector<Node> args;
    };

    Int type_id = 0;
    Variant<NamedInit, ListInit> init;

    constexpr StructNode( Location const& location )
        : LocationHolder( location )
    {
    }
};

struct ArrayNode : LocationHolder
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

    constexpr ArrayNode( Location const& location )
        : LocationHolder( location )
    {
    }
};

struct UnaryNode : LocationHolder
{
    UnaryType type{};
    NodeRef right;

    constexpr UnaryNode( Location const& location )
        : LocationHolder( location )
    {
    }
};

struct OperatorNode : LocationHolder
{
    OperatorType type{};
    Vector<Node> sides;

    constexpr OperatorNode( Location const& location )
        : LocationHolder( location )
    {
    }
};

struct AssignNode : LocationHolder
{
    AssignType type{};
    Vector<Node> sides;

    constexpr AssignNode( Location const& location )
        : LocationHolder( location )
    {
    }
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
    LambdaNode,
    EnumNode,
    StructNode,
    ArrayNode,
    UnaryNode,
    OperatorNode,
    AssignNode >
{
    constexpr NodeType type() const noexcept
    {
        return static_cast<NodeType>( this->index() );
    }

    constexpr Location const& location() const
    {
        switch ( type() )
        {
        case NodeType::NONE:
        case NodeType::SCOPE:
        default:
            return Location::none;

        case NodeType::VARIABLE:
        case NodeType::RETURN:
        case NodeType::BREAK:
        case NodeType::CONTINUE:
        case NodeType::THROW:
        case NodeType::TRY:
        case NodeType::IF:
        case NodeType::SWITCH:
        case NodeType::LOOP:
        case NodeType::WHILE:
        case NodeType::FOR:
        case NodeType::VALUE:
        case NodeType::IDENTIFIER:
        case NodeType::CALL:
        case NodeType::INDEX:
        case NodeType::LAMBDA:
        case NodeType::ENUM:
        case NodeType::STRUCT:
        case NodeType::ARRAY:
        case NodeType::UNARY:
        case NodeType::OPERATOR:
        case NodeType::ASSIGN:
            return reinterpret_cast<LocationHolder const*>( this )->location;
        }
    }
};

Node make_nothing_node( Location const& location );
Node make_bool_node( Location const& location, Bool value );
Node make_int_node( Location const& location, Int value );
Node make_float_node( Location const& location, Float value );
Node make_char_node( Location const& location, Char value );
Node make_string_node( Location const& location, StringRef const& value );
Node make_value_node( Location const& location, Value const& value );
}
