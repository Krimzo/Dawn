#pragma once

#include "id.h"
#include "storage.h"
#include "memory.h"
#include "holder.h"


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
}
