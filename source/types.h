#pragma once

#include "id.h"
#include "node.h"
#include "memory.h"


namespace dawn
{
struct ValueRef;
struct ScopeObject;

enum struct VariableKind
{
    LET = 0,
    VAR,
    REF,
};

struct Variable
{
    VariableKind kind = {};
    Int id = 0;
    Node expr;
};

struct Scope
{
    Array<Node> instr;
};

struct Function
{
    using CppFunc = Func<ValueRef( Array<ValueRef> const& )>;

    Int id = 0;
    RegisterRef<ScopeObject> lambda_parent;
    Array<ValueRef> self_val;
    Array<Variable> args;
    Variant<Scope, CppFunc> body;

    Bool is_lambda() const;
    Bool is_method() const;
    Bool is_unary_op() const;
};

struct Enum
{
    Int id = 0;
    Map<Int, Node> keys_expr;
    Map<Int, ValueRef> keys_value;
};

struct Struct
{
    Int id = 0;
    Array<Variable> fields;
    Array<Function> methods;
};
}
