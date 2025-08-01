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

enum struct FunctionType
{
    GLOBAL = 0,
    METHOD,
    LAMBDA,
};

using CppFuncBody = Func<Value( Value*, Int )>;

struct Function
{
    struct Arg
    {
        VariableKind kind;
        Int id = 0;
    };

    Int id = 0;
    Vector<Arg> args;
    Variant<Scope, CppFuncBody> body;

    Holder<Value> METHOD_self;
    RegisterRef<Frame> LAMBDA_frame;

    FunctionType type() const;
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
