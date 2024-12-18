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
    using CppFunc = Func<ValueRef( Array<ValueRef> const& )>;

    ID name;
    Struct* parent = nullptr;
    Array<Variable> args;
    Array<ValueRef> arg_vals;
    Variant<Scope, CppFunc> body;

    inline Bool is_method() const
    {
        return static_cast<Bool>(parent);
    }

    inline Bool is_unary_op() const
    {
        return args.size() == 1;
    }
};

struct Enum
{
    ID name;
    Map<String, Node> keys_expr;
    Map<Int, ValueRef> keys_value;
};

struct Struct
{
    ID name;
    Array<Variable> fields;
    Array<Function> methods;
};
}
