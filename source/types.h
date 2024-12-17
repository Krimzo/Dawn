#pragma once

#include "node.h"
#include "id.h"


namespace dawn
{
struct Value;
struct ValueRef;

enum struct VariableKind
{
    LET = 0,
    VAR,
    REF,
};

struct Variable
{
    VariableKind kind;
    ID name;
    Node expr;
};

struct Scope
{
    Array<Node> instr;
};

struct Function
{
    using CppFunc = Func<ValueRef( Array<ValueRef> const& )>;

    ID name;
    Array<Variable> args;
    Array<ValueRef> arg_vals;
    Variant<Scope, CppFunc> body;
};

struct Operator : Function
{
    Bool is_unary() const;
};

struct Enum
{
    ID name;
    Map<String, Variable> keys_expr;
    Map<Int, ValueRef> keys_value;
};

struct Struct
{
    ID name;
    Array<Variable> fields;
    Array<Function> methods;
    Array<Operator> operators;

    Variable* get_field( IDSystem& system, Int id );
    Function* get_method( IDSystem& system, Int id );
    Operator* get_operator( IDSystem& system, Int id, Bool is_unary );
};
}
