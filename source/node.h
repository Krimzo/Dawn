#pragma once

#include "stack.h"


namespace dawn
{
struct RefNod
{
    ValueRef value_ref;
};

struct VariableNod
{
    Variable var;
};

struct IdentifierNod
{
    Int id = 0;
};

struct CallNod
{
    NodeRef left_expr;
    Vector<Node> args;
};

struct IndexNod
{
    NodeRef left_expr;
    NodeRef expr;
};

struct ReturnNod
{
    NodeRef expr;
};

struct BreakNod
{
};

struct ContinueNod
{
};

struct ThrowNod
{
    NodeRef expr;
};

struct TryNod
{
    Scope try_scope;
    Int catch_id = 0;
    Scope catch_scope;
};

struct IfNod
{
    template<typename T>
    struct Part // had to cheat the compiler with a template :)
    {
        T expr;
        Scope scope;
    };

    Vector<Part<Node>> parts;
};

struct SwitchNod
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

struct LoopNod
{
    Scope scope;
};

struct WhileNod
{
    NodeRef expr;
    Scope scope;
};

struct ForNod
{
    Variable var;
    NodeRef expr;
    Scope scope;
};

struct EnumNod
{
    Int type_id = 0;
    Int key_id = 0;
};

struct StructNod
{
    Int type_id = 0;
    Vector<Pair<Int, Node>> args;
};

struct ArrayNod
{
    enum struct InitType
    {
        SIZE = 0,
        LIST,
    };

    InitType init_type;
    NodeRef SIZE_value_expr;
    NodeRef SIZE_size_expr;
    Vector<Node> LIST_list;
};

enum struct UnaryType
{
    PLUS = 0,
    MINUS,
    NOT,
};

struct UnaryNod
{
    UnaryType type;
    NodeRef right;
};

enum struct OperatorType
{
    ACCESS = 0,
    POW,
    MOD,
    MUL,
    DIV,
    ADD,
    SUB,
    COMPARE,
    LESS,
    GREAT,
    LESS_EQ,
    GREAT_EQ,
    EQ,
    NOT_EQ,
    AND,
    OR,
    RANGE,
};

struct OperatorNod
{
    OperatorType type;
    Vector<Node> sides;
};

enum struct AssignType
{
    ASSIGN = 0,
    ADD,
    SUB,
    MUL,
    DIV,
    POW,
    MOD,
};

struct AssignNod
{
    AssignType type;
    Vector<Node> sides;
};

enum struct NodeType
{
    EMPTY = 0,

    // instr
    SCOPE,
    VARIABLE,
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

    // expr
    REF,
    IDENTIFIER,
    CALL,
    INDEX,
    ENUM,
    STRUCT,
    ARRAY,
    UNARY,
    OPERATOR,
    ASSIGN,
};

struct NodeHandler
{
    template<typename T>
    static consteval NodeType type()
    {
        if constexpr ( std::is_same_v<T, RefNod> )
            return NodeType::REF;

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

    static void copy( NodeType type, void* to, void const* from )
    {
        switch ( type )
        {
        case NodeType::REF:
            new (to) RefNod( *static_cast<RefNod const*>(from) );
            break;

        case NodeType::VARIABLE:
            new (to) VariableNod( *static_cast<VariableNod const*>(from) );
            break;

        case NodeType::IDENTIFIER:
            new (to) IdentifierNod( *static_cast<IdentifierNod const*>(from) );
            break;

        case NodeType::CALL:
            new (to) CallNod( *static_cast<CallNod const*>(from) );
            break;

        case NodeType::INDEX:
            new (to) IndexNod( *static_cast<IndexNod const*>(from) );
            break;

        case NodeType::RETURN:
            new (to) ReturnNod( *static_cast<ReturnNod const*>(from) );
            break;

        case NodeType::BREAK:
            new (to) BreakNod( *static_cast<BreakNod const*>(from) );
            break;

        case NodeType::CONTINUE:
            new (to) ContinueNod( *static_cast<ContinueNod const*>(from) );
            break;

        case NodeType::THROW:
            new (to) ThrowNod( *static_cast<ThrowNod const*>(from) );
            break;

        case NodeType::TRY:
            new (to) TryNod( *static_cast<TryNod const*>(from) );
            break;

        case NodeType::IF:
            new (to) IfNod( *static_cast<IfNod const*>(from) );
            break;

        case NodeType::SWITCH:
            new (to) SwitchNod( *static_cast<SwitchNod const*>(from) );
            break;

        case NodeType::LOOP:
            new (to) LoopNod( *static_cast<LoopNod const*>(from) );
            break;

        case NodeType::WHILE:
            new (to) WhileNod( *static_cast<WhileNod const*>(from) );
            break;

        case NodeType::FOR:
            new (to) ForNod( *static_cast<ForNod const*>(from) );
            break;

        case NodeType::ENUM:
            new (to) EnumNod( *static_cast<EnumNod const*>(from) );
            break;

        case NodeType::STRUCT:
            new (to) StructNod( *static_cast<StructNod const*>(from) );
            break;

        case NodeType::ARRAY:
            new (to) ArrayNod( *static_cast<ArrayNod const*>(from) );
            break;

        case NodeType::UNARY:
            new (to) UnaryNod( *static_cast<UnaryNod const*>(from) );
            break;

        case NodeType::OPERATOR:
            new (to) OperatorNod( *static_cast<OperatorNod const*>(from) );
            break;

        case NodeType::ASSIGN:
            new (to) AssignNod( *static_cast<AssignNod const*>(from) );
            break;

        case NodeType::SCOPE:
            new (to) Scope( *static_cast<Scope const*>(from) );
            break;
        }
    }

    static void destruct( NodeType type, void* ptr )
    {
        switch ( type )
        {
        case NodeType::REF:
            static_cast<RefNod*>(ptr)->~RefNod();
            break;

        case NodeType::VARIABLE:
            static_cast<VariableNod*>(ptr)->~VariableNod();
            break;

        case NodeType::IDENTIFIER:
            static_cast<IdentifierNod*>(ptr)->~IdentifierNod();
            break;

        case NodeType::CALL:
            static_cast<CallNod*>(ptr)->~CallNod();
            break;

        case NodeType::INDEX:
            static_cast<IndexNod*>(ptr)->~IndexNod();
            break;

        case NodeType::RETURN:
            static_cast<ReturnNod*>(ptr)->~ReturnNod();
            break;

        case NodeType::BREAK:
            static_cast<BreakNod*>(ptr)->~BreakNod();
            break;

        case NodeType::CONTINUE:
            static_cast<ContinueNod*>(ptr)->~ContinueNod();
            break;

        case NodeType::THROW:
            static_cast<ThrowNod*>(ptr)->~ThrowNod();
            break;

        case NodeType::TRY:
            static_cast<TryNod*>(ptr)->~TryNod();
            break;

        case NodeType::IF:
            static_cast<IfNod*>(ptr)->~IfNod();
            break;

        case NodeType::SWITCH:
            static_cast<SwitchNod*>(ptr)->~SwitchNod();
            break;

        case NodeType::LOOP:
            static_cast<LoopNod*>(ptr)->~LoopNod();
            break;

        case NodeType::WHILE:
            static_cast<WhileNod*>(ptr)->~WhileNod();
            break;

        case NodeType::FOR:
            static_cast<ForNod*>(ptr)->~ForNod();
            break;

        case NodeType::ENUM:
            static_cast<EnumNod*>(ptr)->~EnumNod();
            break;

        case NodeType::STRUCT:
            static_cast<StructNod*>(ptr)->~StructNod();
            break;

        case NodeType::ARRAY:
            static_cast<ArrayNod*>(ptr)->~ArrayNod();
            break;

        case NodeType::UNARY:
            static_cast<UnaryNod*>(ptr)->~UnaryNod();
            break;

        case NodeType::OPERATOR:
            static_cast<OperatorNod*>(ptr)->~OperatorNod();
            break;

        case NodeType::ASSIGN:
            static_cast<AssignNod*>(ptr)->~AssignNod();
            break;

        case NodeType::SCOPE:
            static_cast<Scope*>(ptr)->~Scope();
            break;
        }
    }
};

consteval size_t max_nod_size()
{
    return std::max( {
        sizeof( RefNod ),
        sizeof( VariableNod ),
        sizeof( IdentifierNod ),
        sizeof( CallNod ),
        sizeof( IndexNod ),
        sizeof( ReturnNod ),
        sizeof( BreakNod ),
        sizeof( ContinueNod ),
        sizeof( ThrowNod ),
        sizeof( TryNod ),
        sizeof( IfNod ),
        sizeof( SwitchNod ),
        sizeof( LoopNod ),
        sizeof( WhileNod ),
        sizeof( ForNod ),
        sizeof( EnumNod ),
        sizeof( StructNod ),
        sizeof( ArrayNod ),
        sizeof( UnaryNod ),
        sizeof( OperatorNod ),
        sizeof( AssignNod ),
        sizeof( Scope ) } );
}

consteval size_t max_nod_align()
{
    return std::max( {
        alignof(RefNod),
        alignof(VariableNod),
        alignof(IdentifierNod),
        alignof(CallNod),
        alignof(IndexNod),
        alignof(ReturnNod),
        alignof(BreakNod),
        alignof(ContinueNod),
        alignof(ThrowNod),
        alignof(TryNod),
        alignof(IfNod),
        alignof(SwitchNod),
        alignof(LoopNod),
        alignof(WhileNod),
        alignof(ForNod),
        alignof(EnumNod),
        alignof(StructNod),
        alignof(ArrayNod),
        alignof(UnaryNod),
        alignof(OperatorNod),
        alignof(AssignNod),
        alignof(Scope) } );
}

struct Node : Storage<max_nod_size(), max_nod_align(), NodeType, NodeHandler>
{
};

Node make_nothing_node();
Node make_bool_node( Bool value );
Node make_int_node( Int value );
Node make_float_node( Float value );
Node make_char_node( Char value );
Node make_string_node( StringRef const& value );
Node make_ref_node( ValueRef const& value );
}
