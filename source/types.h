#pragma once

#include "node.h"
#include "id.h"


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
    ID name;
    Node expr;
};

struct Scope
{
    Array<Node> instr;
};

struct Function
{
    using CppFunc = Func<Value( Array<ValueRef> const& )>;

    ID name;
    Array<Variable> args;
    Variant<Scope, CppFunc> body;
};

struct Enum
{
    ID name;
    Map<String, Variable> keys_expr;
    Map<Int, ValueRef> keys_value;
};

struct Struct
{
    ID name;
    Array<Variable> fields;
    Array<Function> methods;

    Variable* get_field( IDSystem& system, Int id );
    Function* get_method( IDSystem& system, Int id );
};
}
