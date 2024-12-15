#pragma once

#include "syntax.h"
#include "storage.h"


namespace dawn
{
struct RefNod;
struct CastNod;
struct VariableNod;
struct IdentifierNod;
struct CallNod;
struct IndexNod;
struct ReturnNod;
struct BreakNod;
struct ContinueNod;
struct ThrowNod;
struct TryNod;
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
    REF,
    CAST,
    VARIABLE,
    IDENTIFIER,
    CALL,
    INDEX,
    RETURN,
    BREAK,
    CONTINUE,
    THROW,
    TRY,
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

struct NodeHandler
{
    template<typename T>
    static consteval NodeType type()
    {
        if constexpr ( std::is_same_v<T, RefNod> )
            return NodeType::REF;

        else if constexpr ( std::is_same_v<T, CastNod> )
            return NodeType::CAST;

        else if constexpr ( std::is_same_v<T, VariableNod> )
            return NodeType::VARIABLE;

        else if constexpr ( std::is_same_v<T, IdentifierNod> )
            return NodeType::IDENTIFIER;

        else if constexpr ( std::is_same_v<T, CallNod> )
            return NodeType::CALL;

        else if constexpr ( std::is_same_v<T, IndexNod> )
            return NodeType::INDEX;

        else if constexpr ( std::is_same_v<T, ReturnNod> )
            return NodeType::RETURN;

        else if constexpr ( std::is_same_v<T, BreakNod> )
            return NodeType::BREAK;

        else if constexpr ( std::is_same_v<T, ContinueNod> )
            return NodeType::CONTINUE;

        else if constexpr ( std::is_same_v<T, ThrowNod> )
            return NodeType::THROW;

        else if constexpr ( std::is_same_v<T, TryNod> )
            return NodeType::TRY;

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

    static void copy( NodeType type, void*& to, void const* from );

    static void destruct( NodeType type, void* ptr );
};

struct Node : private DynStorage<NodeType, NodeHandler>
{
    constexpr NodeType type() const
    {
        return DynStorage::type();
    }

    template<typename T>
    constexpr T const& as() const
    {
        return DynStorage::as<T>();
    }

    template<typename T>
    constexpr T& as()
    {
        return DynStorage::as<T>();
    }

    template<typename T, typename... Args>
    constexpr T& store( Args const&... args )
    {
        return DynStorage::emplace<T>( args... );
    }
};
}
