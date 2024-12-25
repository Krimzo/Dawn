#pragma once

#include "type.h"


namespace dawn
{
struct ValueRef;
struct ScopeObject;
struct Instruction;

struct BVariable
{
    VariableKind kind;
    Int id = 0;
    Vector<Instruction> expr;
};

struct BFunction
{
    using DawnFunc = Vector<Instruction>;
    using CppFunc = Func<ValueRef( Vector<ValueRef> const& )>;

    Int id = 0;
    RegisterRef<ScopeObject> lambda_parent;
    Vector<ValueRef> self_val;
    Vector<BVariable> args;
    Variant<DawnFunc, CppFunc> body;

    Bool is_lambda() const;
    Bool is_method() const;
    Bool is_unary_op() const;
};

struct BEnum
{
    Int id = 0;
    Map<Int, Vector<Instruction>> keys_expr;
    Map<Int, ValueRef> keys_value;
};

struct BStruct
{
    Int id = 0;
    Vector<Variable> fields;
    Vector<Function> methods;
};
}
