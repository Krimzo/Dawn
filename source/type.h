#pragma once

#include "id.h"
#include "storage.h"
#include "memory.h"


namespace dawn
{
struct Node;
using NodeRef = RegisterRef<Node>;

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
    VariableKind kind;
    Int id = 0;
    NodeRef expr;
};

struct Scope
{
    Vector<Node> instr;
};

struct Function
{
    using CppFunc = Func<ValueRef( ValueRef const*, Int )>;

    Int id = 0;
    RegisterRef<ScopeObject> lambda_parent;
    Vector<ValueRef> self_vals;
    Vector<Variable> args;
    Variant<Scope, CppFunc> body;

    Bool is_lambda() const;
    Bool is_method() const;
    Bool is_unary_op() const;
};

struct Enum
{
    Int id = 0;
    OMap<Int, Node> keys_expr;
    OMap<Int, ValueRef> keys_value;
};

struct Struct
{
    Int id = 0;
    Vector<Variable> fields;
    Vector<Function> methods;
};
}
