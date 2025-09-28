#pragma once

#include "id.h"
#include "memory.h"
#include "holder.h"
#include "token.h"
#include "decl.h"


namespace dawn
{
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
    struct Arg
    {
        VariableKind kind;
        Int id = 0;
    };

    Int id = 0;
    Vector<Arg> args;
    Scope body;
};

struct Enum
{
    struct Entry
    {
        Int id = 0;
        Variant<NodeRef, Holder<Value>> expr;
    };

    Int id = 0;
    Vector<Entry> entries;

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
}
