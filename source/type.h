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
    ID type_id;
    VarKind kind{};
};

struct Variable
{
    VarType type;
    ID id;
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
        ID id;
    };

    ID id;
    Vector<Arg> args;
    Scope body;
};

struct Enum
{
    struct Entry
    {
        ID id;
        Variant<NodeRef, Holder<Value>> expr;
    };

    ID id;
    Vector<Entry> entries;

    Entry* get( ID id );
};

struct Struct
{
    struct Field
    {
        ID id;
        ID type_id;
    };

    ID id;
    Vector<Field> fields;
    Vector<Function> methods;

    Bool contains( ID id ) const;
};

std::ostream& operator<<( std::ostream& stream, VarKind kind );
std::ostream& operator<<( std::ostream& stream, VarType const& type );
}
