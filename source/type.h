#pragma once

#include "id.h"
#include "storage.h"
#include "memory.h"
#include "holder.h"


namespace dawn
{
struct Node;
using NodeRef = RegisterRef<Node>;

struct Value;
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
    using CppFunc = Func<Value( Value const*, Int )>;

    struct Arg
    {
        VariableKind kind;
        Int id = 0;
    };

    Int id = 0;
    Vector<Arg> args;
    Variant<Scope, CppFunc> body;

    RegisterRef<Frame> frame;
    Holder<Value> self;

    Bool is_lambda() const;
    Bool is_method() const;
};

struct Enum
{
    Int id = 0;
    OMap<Int, Node> keys_expr;
    OMap<Int, Value> keys_value;
};

struct Struct
{
    struct Field
    {
        Int id = 0;
        NodeRef expr;
    };

    Int id = 0;
    OMap<Int, Field> fields;
    Vector<Int> field_order;
    OMap<Int, Function> methods;
};
}
