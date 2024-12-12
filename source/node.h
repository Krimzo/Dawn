#pragma once

#include "syntax.h"


namespace dawn
{
struct ValueNod;
struct BoxNod;
struct CastNod;
struct VariableNod;
struct IdentifierNod;
struct FunctionNod;
struct ReturnNod;
struct BreakNod;
struct ContinueNod;
struct IfNod;
struct SwitchNod;
struct LoopNod;
struct WhileNod;
struct ForNod;
struct EnumNod;
struct StructNod;
struct ArrayNod;
struct UnaryNod;
struct OperatorNod;
struct AssignNod;
struct Scope;

enum struct NodeType
{
    EMPTY = 0,
    VALUE,
    BOX,
    CAST,
    VARIABLE,
    IDENTIFIER,
    FUNCTION,
    RETURN,
    BREAK,
    CONTINUE,
    IF,
    SWITCH,
    LOOP,
    WHILE,
    FOR,
    ENUM,
    STRUCT,
    ARRAY,
    UNARY,
    OPERATOR,
    ASSIGN,
    SCOPE,
};

template<typename T>
consteval NodeType node_type()
{
    if constexpr ( std::is_same_v<T, ValueNod> )
        return NodeType::VALUE;

    else if constexpr ( std::is_same_v<T, BoxNod> )
        return NodeType::BOX;

    else if constexpr ( std::is_same_v<T, CastNod> )
        return NodeType::CAST;

    else if constexpr ( std::is_same_v<T, VariableNod> )
        return NodeType::VARIABLE;

    else if constexpr ( std::is_same_v<T, IdentifierNod> )
        return NodeType::IDENTIFIER;

    else if constexpr ( std::is_same_v<T, FunctionNod> )
        return NodeType::FUNCTION;

    else if constexpr ( std::is_same_v<T, ReturnNod> )
        return NodeType::RETURN;

    else if constexpr ( std::is_same_v<T, BreakNod> )
        return NodeType::BREAK;

    else if constexpr ( std::is_same_v<T, ContinueNod> )
        return NodeType::CONTINUE;

    else if constexpr ( std::is_same_v<T, IfNod> )
        return NodeType::IF;

    else if constexpr ( std::is_same_v<T, SwitchNod> )
        return NodeType::SWITCH;

    else if constexpr ( std::is_same_v<T, LoopNod> )
        return NodeType::LOOP;

    else if constexpr ( std::is_same_v<T, WhileNod> )
        return NodeType::WHILE;

    else if constexpr ( std::is_same_v<T, ForNod> )
        return NodeType::FOR;

    else if constexpr ( std::is_same_v<T, EnumNod> )
        return NodeType::ENUM;

    else if constexpr ( std::is_same_v<T, StructNod> )
        return NodeType::STRUCT;

    else if constexpr ( std::is_same_v<T, ArrayNod> )
        return NodeType::ARRAY;

    else if constexpr ( std::is_same_v<T, UnaryNod> )
        return NodeType::UNARY;

    else if constexpr ( std::is_same_v<T, OperatorNod> )
        return NodeType::OPERATOR;

    else if constexpr ( std::is_same_v<T, AssignNod> )
        return NodeType::ASSIGN;

    else if constexpr ( std::is_same_v<T, Scope> )
        return NodeType::SCOPE;

    else
        static_assert(false, "Invalid node type");
}

struct Node : private Any
{
    constexpr NodeType type() const
    {
        return m_type;
    }

    template<typename T>
    constexpr T const& as() const
    {
        return *_Cast<T>();
    }

    template<typename T>
    constexpr T& as()
    {
        return *_Cast<T>();
    }

    template<typename T>
    constexpr T& store()
    {
        m_type = node_type<T>();
        return emplace<T>();
    }

private:
    NodeType m_type = NodeType::EMPTY;
};
}
