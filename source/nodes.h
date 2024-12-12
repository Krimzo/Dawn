#pragma once

#include "values.h"


namespace dawn
{
struct RefNod
{
    ValueRef value_ref;
};

struct CastNod
{
    String type;
    Node expr;
};

struct VariableNod
{
    Variable var;
};

struct IdentifierNod
{
    String name;
};

struct FunctionNod
{
    String name;
    Array<Node> args;
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
    String type;
    String key;
};

struct StructNod
{
    String type;
    Map<String, Node> args;
};

struct ArrayNod
{
    enum struct InitType
    {
        SIZE,
        LIST,
    };

    InitType init_type;
    Node SIZE_value_expr;
    Node SIZE_size_expr;
    Array<Node> LIST_list;
};

enum struct UnaryType
{
    PLUS,
    MINUS,
    NOT,
    RANGE,
};

struct UnaryNod
{
    UnaryType type;
    Node right;
};

enum struct OperatorType
{
    ACCESS,
    RANGE,
    POW,
    MOD,
    MUL,
    DIV,
    ADD,
    SUB,
    LESS,
    GREAT,
    LESS_EQ,
    GREAT_EQ,
    EQ,
    NOT_EQ,
    AND,
    OR,
};

struct OperatorNod
{
    OperatorType type;
    Node left;
    Node right;
};

enum struct AssignType
{
    ASSIGN,
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
Node make_string_node( StringRef const& value );
Node make_value_node( Value const& value );
}
