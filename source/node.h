#pragma once

#include "syntax.h"


namespace dawn
{
enum struct NodeType
{
    EMPTY,
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

struct Node : private std::any
{
    NodeType type() const;

    template<typename T>
    constexpr T& store()
    {
        return emplace<T>();
    }

    template<typename T>
    constexpr T& as()
    {
        return *_Cast<T>();
    }

    template<typename T>
    constexpr T const& as() const
    {
        return *_Cast<T>();
    }
};
}
