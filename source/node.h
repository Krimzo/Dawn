#pragma once

#include "stack.h"

namespace dawn
{
struct LocationHolder
{
    Location location;

    explicit constexpr LocationHolder(Location location) : location(location)
    {
    }
};

struct None
{
    constexpr None() = default;
};

struct VariableNode : LocationHolder
{
    Variable var;

    constexpr VariableNode(Location location) : LocationHolder(location)
    {
    }
};

struct IdentifierNode : LocationHolder
{
    ID id;

    constexpr IdentifierNode(Location location) : LocationHolder(location)
    {
    }
};

struct CallNode : LocationHolder
{
    NodeRef left_expr;
    Vector<Node> args;

    constexpr CallNode(Location location) : LocationHolder(location)
    {
    }
};

struct IndexNode : LocationHolder
{
    NodeRef left_expr;
    NodeRef expr;

    constexpr IndexNode(Location location) : LocationHolder(location)
    {
    }
};

struct ReturnNode : LocationHolder
{
    NodeRef expr;

    constexpr ReturnNode(Location location) : LocationHolder(location)
    {
    }
};

struct BreakNode : LocationHolder
{
    constexpr BreakNode(Location location) : LocationHolder(location)
    {
    }
};

struct ContinueNode : LocationHolder
{
    constexpr ContinueNode(Location location) : LocationHolder(location)
    {
    }
};

struct ThrowNode : LocationHolder
{
    NodeRef expr;

    constexpr ThrowNode(Location location) : LocationHolder(location)
    {
    }
};

struct TryNode : LocationHolder
{
    Scope try_scope;
    ID catch_id;
    Scope catch_scope;

    constexpr TryNode(Location location) : LocationHolder(location)
    {
    }
};

struct IfNode : LocationHolder
{
    template <typename T> // Template is required because Node does not exist at this stage.
    struct Part
    {
        T expr;
        Scope scope;
    };

    Vector<Part<Node>> parts;

    constexpr IfNode(Location location) : LocationHolder(location)
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

    constexpr SwitchNode(Location location) : LocationHolder(location)
    {
    }
};

struct LoopNode : LocationHolder
{
    Scope scope;

    constexpr LoopNode(Location location) : LocationHolder(location)
    {
    }
};

struct WhileNode : LocationHolder
{
    NodeRef expr;
    Scope scope;

    constexpr WhileNode(Location location) : LocationHolder(location)
    {
    }
};

struct ForNode : LocationHolder
{
    ID var_id;
    NodeRef expr;
    Scope scope;

    constexpr ForNode(Location location) : LocationHolder(location)
    {
    }
};

struct LambdaNode : LocationHolder
{
    Value func_value;

    constexpr LambdaNode(Location location) : LocationHolder(location)
    {
    }
};

struct EnumNode : LocationHolder
{
    ID type_id;
    ID key_id;

    constexpr EnumNode(Location location) : LocationHolder(location)
    {
    }
};

struct StructNode : LocationHolder
{
    struct NamedInit
    {
        Map<ID, Node> args;
    };

    struct ListInit
    {
        Vector<Node> args;
    };

    ID type_id;
    Variant<NamedInit, ListInit> init;

    constexpr StructNode(Location location) : LocationHolder(location)
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
        ID type_id;
        NodeRef size_expr;
    };

    Variant<ListInit, SizedInit> init;

    constexpr ArrayNode(Location location) : LocationHolder(location)
    {
    }
};

struct AccessNode : LocationHolder
{
    NodeRef left_expr;
    ID right_id;

    constexpr AccessNode(Location location) : LocationHolder(location)
    {
    }
};

struct CastNode : LocationHolder
{
    NodeRef left_expr;
    ID right_type_id;

    constexpr CastNode(Location location) : LocationHolder(location)
    {
    }
};

struct OperatorNode : LocationHolder
{
    OperatorType type{};
    Vector<Node> sides;

    constexpr OperatorNode(Location location) : LocationHolder(location)
    {
    }
};

struct AssignNode : LocationHolder
{
    AssignType type{};
    Vector<Node> sides;

    constexpr AssignNode(Location location) : LocationHolder(location)
    {
    }
};

struct Node : Variant<None, Scope, VariableNode, ReturnNode, BreakNode, ContinueNode, ThrowNode, TryNode, IfNode,
                      SwitchNode, LoopNode, WhileNode, ForNode, Value, IdentifierNode, CallNode, IndexNode, LambdaNode,
                      EnumNode, StructNode, ArrayNode, AccessNode, OperatorNode, AssignNode, CastNode>
{
    constexpr NodeType type() const noexcept
    {
        return static_cast<NodeType>(this->index());
    }

    constexpr Location location() const
    {
        switch (type())
        {
        case NodeType::NONE:
        case NodeType::SCOPE:
        default:
            return DEFAULT_LOCATION;

        case NodeType::VALUE:
            return std::get<Value>(*this).location();

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
        case NodeType::IDENTIFIER:
        case NodeType::CALL:
        case NodeType::INDEX:
        case NodeType::LAMBDA:
        case NodeType::ENUM:
        case NodeType::STRUCT:
        case NodeType::ARRAY:
        case NodeType::ACCESS:
        case NodeType::OPERATOR:
        case NodeType::ASSIGN:
        case NodeType::CAST:
            return reinterpret_cast<LocationHolder const*>(this)->location;
        }
    }

  private:
    static constexpr Location DEFAULT_LOCATION{};
};
} // namespace dawn
