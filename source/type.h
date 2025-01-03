#pragma once

#include "id.h"
#include "storage.h"
#include "memory.h"
#include "holder.h"


namespace dawn
{
struct Node;
using NodeRef = RegisterRef<Node>;

struct ValueRef;
struct Frame;

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

    struct Arg
    {
        VariableKind kind;
        Int id = 0;
    };

    Int id = 0;
    Vector<Arg> args;
    Variant<Scope, CppFunc> body;

    RegisterRef<Frame> frame;
    Holder<ValueRef> self;

    Bool is_lambda() const;
    Bool is_method() const;
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
    OMap<Int, Variable> fields;
    OMap<Int, Function> methods;
};
}
