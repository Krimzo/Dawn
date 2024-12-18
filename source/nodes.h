#pragma once

#include "values.h"


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
    ID name;
};

struct CallNod
{
    Node left_expr;
    Array<Node> args;
    Array<ValueRef> arg_vals;
};

struct IndexNod
{
    Node left_expr;
    Node expr;
};

struct ReturnNod
{
    Node expr;
};

struct BreakNod
{
};

struct ContinueNod
{
};

struct ThrowNod
{
    Node expr;
};

struct TryNod
{
    Scope try_scope;
    ID catch_name;
    Scope catch_scope;
};

struct IfNod
{
    struct Part
    {
        Node expr;
        Scope scope;
    };

    Part if_part;
    Array<Part> elif_parts;
    Opt<Part> else_part;
};

struct SwitchNod
{
    struct Part
    {
        Array<Node> exprs;
        Scope scope;
    };

    Node main_expr;
    Array<Part> cases;
    Opt<Scope> def_scope;
};

struct LoopNod
{
    Scope scope;
};

struct WhileNod
{
    Node expr;
    Scope scope;
};

struct ForNod
{
    Variable var;
    Node expr;
    Scope scope;
};

struct EnumNod
{
    ID type;
    ID key;
};

struct StructNod
{
    ID type;
    Array<Pair<ID, Node>> args;
};

struct ArrayNod
{
    enum struct InitType
    {
        SIZE = 0,
        LIST,
    };

    InitType init_type;
    Node SIZE_value_expr;
    Node SIZE_size_expr;
    Array<Node> LIST_list;
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
    Node right;
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
    Node left;
    Node right;
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
    Node left;
    Node right;
};

Node make_nothing_node();
Node make_bool_node( Bool value );
Node make_int_node( Int value );
Node make_float_node( Float value );
Node make_char_node( Char value );
Node make_string_node( String value );
Node make_value_node( ValueRef const& value );
}
