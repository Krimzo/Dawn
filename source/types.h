#pragma once

#include "node.h"


namespace dawn
{
struct Value;
struct ValueRef;

enum struct VariableKind
{
    LET = 0,
    VAR,
    REF,
};

struct Variable
{
    VariableKind kind;
    String name;
    Node expr;
};

struct Scope
{
    Array<Node> instr;
};

struct Function
{
    using CppFunc = Func<Value( Array<ValueRef> const& )>;

    String name;
    Array<Variable> args;
    Variant<Scope, CppFunc> body;
};

struct Enum
{
    String name;
    Map<String, Variable> keys_expr;
    Map<String, ValueRef> keys_value;
};

struct Struct
{
    String name;
    Array<Variable> fields;
    Array<Function> methods;

    Function const* get_method( StringRef const& name ) const;
};
}
