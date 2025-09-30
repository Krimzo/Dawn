#pragma once

#include "id.h"
#include "memory.h"
#include "holder.h"
#include "token.h"
#include "decl.h"


namespace dawn
{
struct VarType
{
    enum struct Kind
    {
        CONSTANT = 0,
        VARIABLE,
        REFERENCE,
    };

    Int type_id = 0;
    Kind kind{};
};

struct Variable
{
    VarType type;
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
        VarType type;
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
        Int type_id = 0;
    };

    Int id = 0;
    Vector<Field> fields;
    Vector<Function> methods;

    Bool contains( Int id ) const;
};

std::ostream& operator<<( std::ostream& stream, VarType::Kind kind );
std::ostream& operator<<( std::ostream& stream, VarType const& type );
}
