#pragma once

#include "id.h"
#include "storage.h"
#include "memory.h"


namespace dawn
{
struct Node;
using NodeRef = RegisterRef<Node>;

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
    Int id = 0;
    Vector<Variable> args;
    Scope body;
};

struct Enum
{
    Int id = 0;
    Map<Int, Node> keys_expr;
};

struct Struct
{
    Int id = 0;
    Vector<Variable> fields;
    Vector<Function> methods;
};
}
