#pragma once

#include "stack.h"


namespace dawn
{
struct RefNode
{
    Value value_ref;
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

enum struct ArrayType
{
    LIST = 0,
    SIZE,
};

struct ArrayNode
{
    ArrayType type;

    // list init
    Vector<Node> LIST_list;

    // sized init
    Int SIZE_typeid = 0;
    NodeRef SIZE_expr;
};

enum struct UnaryType
{
    PLUS = 0,
    MINUS,
    NOT,
};

struct UnaryNode
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

struct OperatorNode
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

struct AssignNode
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
        if constexpr ( std::is_same_v<T, RefNode> )
            return NodeType::REF;

        else if constexpr ( std::is_same_v<T, VariableNode> )
            return NodeType::VARIABLE;

        else if constexpr ( std::is_same_v<T, IdentifierNode> )
            return NodeType::IDENTIFIER;

        else if constexpr ( std::is_same_v<T, CallNode> )
            return NodeType::CALL;

        else if constexpr ( std::is_same_v<T, IndexNode> )
            return NodeType::INDEX;

        else if constexpr ( std::is_same_v<T, ReturnNode> )
            return NodeType::RETURN;

        else if constexpr ( std::is_same_v<T, BreakNode> )
            return NodeType::BREAK;

        else if constexpr ( std::is_same_v<T, ContinueNode> )
            return NodeType::CONTINUE;

        else if constexpr ( std::is_same_v<T, ThrowNode> )
            return NodeType::THROW;

        else if constexpr ( std::is_same_v<T, TryNode> )
            return NodeType::TRY;

        else if constexpr ( std::is_same_v<T, IfNode> )
            return NodeType::IF;

        else if constexpr ( std::is_same_v<T, SwitchNode> )
            return NodeType::SWITCH;

        else if constexpr ( std::is_same_v<T, LoopNode> )
            return NodeType::LOOP;

        else if constexpr ( std::is_same_v<T, WhileNode> )
            return NodeType::WHILE;

        else if constexpr ( std::is_same_v<T, ForNode> )
            return NodeType::FOR;

        else if constexpr ( std::is_same_v<T, EnumNode> )
            return NodeType::ENUM;

        else if constexpr ( std::is_same_v<T, StructNode> )
            return NodeType::STRUCT;

        else if constexpr ( std::is_same_v<T, ArrayNode> )
            return NodeType::ARRAY;

        else if constexpr ( std::is_same_v<T, UnaryNode> )
            return NodeType::UNARY;

        else if constexpr ( std::is_same_v<T, OperatorNode> )
            return NodeType::OPERATOR;

        else if constexpr ( std::is_same_v<T, AssignNode> )
            return NodeType::ASSIGN;

        else if constexpr ( std::is_same_v<T, Scope> )
            return NodeType::SCOPE;

        else
            static_assert( false, "Invalid node type" );
    }

    static void copy( NodeType type, void* to, void const* from )
    {
        switch ( type )
        {
        case NodeType::REF:
            new ( to ) RefNode( *static_cast<RefNode const*>( from ) );
            break;

        case NodeType::VARIABLE:
            new ( to ) VariableNode( *static_cast<VariableNode const*>( from ) );
            break;

        case NodeType::IDENTIFIER:
            new ( to ) IdentifierNode( *static_cast<IdentifierNode const*>( from ) );
            break;

        case NodeType::CALL:
            new ( to ) CallNode( *static_cast<CallNode const*>( from ) );
            break;

        case NodeType::INDEX:
            new ( to ) IndexNode( *static_cast<IndexNode const*>( from ) );
            break;

        case NodeType::RETURN:
            new ( to ) ReturnNode( *static_cast<ReturnNode const*>( from ) );
            break;

        case NodeType::BREAK:
            new ( to ) BreakNode( *static_cast<BreakNode const*>( from ) );
            break;

        case NodeType::CONTINUE:
            new ( to ) ContinueNode( *static_cast<ContinueNode const*>( from ) );
            break;

        case NodeType::THROW:
            new ( to ) ThrowNode( *static_cast<ThrowNode const*>( from ) );
            break;

        case NodeType::TRY:
            new ( to ) TryNode( *static_cast<TryNode const*>( from ) );
            break;

        case NodeType::IF:
            new ( to ) IfNode( *static_cast<IfNode const*>( from ) );
            break;

        case NodeType::SWITCH:
            new ( to ) SwitchNode( *static_cast<SwitchNode const*>( from ) );
            break;

        case NodeType::LOOP:
            new ( to ) LoopNode( *static_cast<LoopNode const*>( from ) );
            break;

        case NodeType::WHILE:
            new ( to ) WhileNode( *static_cast<WhileNode const*>( from ) );
            break;

        case NodeType::FOR:
            new ( to ) ForNode( *static_cast<ForNode const*>( from ) );
            break;

        case NodeType::ENUM:
            new ( to ) EnumNode( *static_cast<EnumNode const*>( from ) );
            break;

        case NodeType::STRUCT:
            new ( to ) StructNode( *static_cast<StructNode const*>( from ) );
            break;

        case NodeType::ARRAY:
            new ( to ) ArrayNode( *static_cast<ArrayNode const*>( from ) );
            break;

        case NodeType::UNARY:
            new ( to ) UnaryNode( *static_cast<UnaryNode const*>( from ) );
            break;

        case NodeType::OPERATOR:
            new ( to ) OperatorNode( *static_cast<OperatorNode const*>( from ) );
            break;

        case NodeType::ASSIGN:
            new ( to ) AssignNode( *static_cast<AssignNode const*>( from ) );
            break;

        case NodeType::SCOPE:
            new ( to ) Scope( *static_cast<Scope const*>( from ) );
            break;
        }
    }

    static void destruct( NodeType type, void* ptr )
    {
        switch ( type )
        {
        case NodeType::REF:
            static_cast<RefNode*>( ptr )->~RefNode();
            break;

        case NodeType::VARIABLE:
            static_cast<VariableNode*>( ptr )->~VariableNode();
            break;

        case NodeType::IDENTIFIER:
            static_cast<IdentifierNode*>( ptr )->~IdentifierNode();
            break;

        case NodeType::CALL:
            static_cast<CallNode*>( ptr )->~CallNode();
            break;

        case NodeType::INDEX:
            static_cast<IndexNode*>( ptr )->~IndexNode();
            break;

        case NodeType::RETURN:
            static_cast<ReturnNode*>( ptr )->~ReturnNode();
            break;

        case NodeType::BREAK:
            static_cast<BreakNode*>( ptr )->~BreakNode();
            break;

        case NodeType::CONTINUE:
            static_cast<ContinueNode*>( ptr )->~ContinueNode();
            break;

        case NodeType::THROW:
            static_cast<ThrowNode*>( ptr )->~ThrowNode();
            break;

        case NodeType::TRY:
            static_cast<TryNode*>( ptr )->~TryNode();
            break;

        case NodeType::IF:
            static_cast<IfNode*>( ptr )->~IfNode();
            break;

        case NodeType::SWITCH:
            static_cast<SwitchNode*>( ptr )->~SwitchNode();
            break;

        case NodeType::LOOP:
            static_cast<LoopNode*>( ptr )->~LoopNode();
            break;

        case NodeType::WHILE:
            static_cast<WhileNode*>( ptr )->~WhileNode();
            break;

        case NodeType::FOR:
            static_cast<ForNode*>( ptr )->~ForNode();
            break;

        case NodeType::ENUM:
            static_cast<EnumNode*>( ptr )->~EnumNode();
            break;

        case NodeType::STRUCT:
            static_cast<StructNode*>( ptr )->~StructNode();
            break;

        case NodeType::ARRAY:
            static_cast<ArrayNode*>( ptr )->~ArrayNode();
            break;

        case NodeType::UNARY:
            static_cast<UnaryNode*>( ptr )->~UnaryNode();
            break;

        case NodeType::OPERATOR:
            static_cast<OperatorNode*>( ptr )->~OperatorNode();
            break;

        case NodeType::ASSIGN:
            static_cast<AssignNode*>( ptr )->~AssignNode();
            break;

        case NodeType::SCOPE:
            static_cast<Scope*>( ptr )->~Scope();
            break;
        }
    }
};

consteval size_t max_node_size()
{
    return std::max( {
        sizeof( RefNode ),
        sizeof( VariableNode ),
        sizeof( IdentifierNode ),
        sizeof( CallNode ),
        sizeof( IndexNode ),
        sizeof( ReturnNode ),
        sizeof( BreakNode ),
        sizeof( ContinueNode ),
        sizeof( ThrowNode ),
        sizeof( TryNode ),
        sizeof( IfNode ),
        sizeof( SwitchNode ),
        sizeof( LoopNode ),
        sizeof( WhileNode ),
        sizeof( ForNode ),
        sizeof( EnumNode ),
        sizeof( StructNode ),
        sizeof( ArrayNode ),
        sizeof( UnaryNode ),
        sizeof( OperatorNode ),
        sizeof( AssignNode ),
        sizeof( Scope ) } );
}

consteval size_t max_node_align()
{
    return std::max( {
        alignof( RefNode ),
        alignof( VariableNode ),
        alignof( IdentifierNode ),
        alignof( CallNode ),
        alignof( IndexNode ),
        alignof( ReturnNode ),
        alignof( BreakNode ),
        alignof( ContinueNode ),
        alignof( ThrowNode ),
        alignof( TryNode ),
        alignof( IfNode ),
        alignof( SwitchNode ),
        alignof( LoopNode ),
        alignof( WhileNode ),
        alignof( ForNode ),
        alignof( EnumNode ),
        alignof( StructNode ),
        alignof( ArrayNode ),
        alignof( UnaryNode ),
        alignof( OperatorNode ),
        alignof( AssignNode ),
        alignof( Scope ) } );
}

struct Node : Storage<max_node_size(), max_node_align(), NodeType, NodeHandler>
{
};

Node make_nothing_node();
Node make_bool_node( Bool value );
Node make_int_node( Int value );
Node make_float_node( Float value );
Node make_char_node( Char value );
Node make_string_node( StringRef const& value );
Node make_ref_node( Value const& value );
}
