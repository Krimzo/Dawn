#pragma once

#include "node.h"
#include "id.h"


namespace dawn
{
struct Value;
struct ValueRef;
struct Struct;

enum struct VariableKind
{
    LET = 0,
    VAR,
    REF,
};

struct Variable
{
    VariableKind kind = {};
    Int id = -1;
    Node expr;
};

struct Scope
{
    Array<Node> instr;
};

struct Function
{
    using CppFunc = Func<ValueRef( Array<ValueRef> const& )>;

    Int id = -1;
    Struct* parent = nullptr;
    Array<ValueRef> self_val;
    Array<Variable> args;
    Variant<Scope, CppFunc> body;

    inline Bool is_method() const
    {
        return !self_val.empty();
    }

    inline Bool is_unary_op() const
    {
        return args.size() == 1;
    }
};

struct Enum
{
    Int id = -1;
    Map<Int, Node> keys_expr;
    Map<Int, ValueRef> keys_value;
};

struct Struct
{
    Int id = -1;
    Array<Variable> fields;
    Array<Function> methods;
};
}
