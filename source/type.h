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
    CONST = 0,
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
    using CppFunc = Func<Value( Value*, Int )>;

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
    struct Entry
    {
        Int id = 0;
        NodeRef expr;
    };

    Int id = 0;
    Vector<Entry> entries;

    Bool contains( Int id ) const;
    Entry* get( Int id );
};

struct Struct
{
    struct Field
    {
        Int id = 0;
        NodeRef expr;
    };

    Int id = 0;
    Vector<Field> fields;
    Vector<Function> methods;

    Bool contains( Int id ) const;
};

std::ostream& operator<<( std::ostream& stream, VariableKind kind );
}
