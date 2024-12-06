#pragma once

#include "valtype.h"


namespace dawn
{
// sub-types
struct Scope : Node
{
    Array<Ref<Node>> instr;
};

struct Variable
{
    String name;
    Bool is_var = false;
    Ref<Type> type;
    Ref<Node> expr;
};

struct Argument
{
    String name;
    Ref<Type> type;
};

struct Function
{
    using CppFunc = Func<Ref<Value>( Array<Ref<Value>> const& )>;

    String name;
    Array<Argument> args;
    Ref<Type> type;
    Variant<Scope, CppFunc> body;
};

struct Operator : Function
{
};

struct Method : Function
{
    Bool is_var = false;
};

// types
struct BoolType : Type
{
    BoolType() { name = tp_bool; }

    Ref<Value> construct() const override;
};

struct IntType : Type
{
    IntType() { name = tp_int; }

    Ref<Value> construct() const override;
};

struct FloatType : Type
{
    FloatType() { name = tp_float; }

    Ref<Value> construct() const override;
};

struct CharType : Type
{
    CharType() { name = tp_char; }

    Ref<Value> construct() const override;
};

struct StringType : Type
{
    StringType() { name = tp_string; }

    Ref<Value> construct() const override;
};

struct EnumType : Type
{
    Ref<Type> val_type;
    Array<Pair<String, Ref<Value>>> keys;

    Ref<Value> construct() const override;
};

struct LayerType : Type
{
    Array<Method> methods;

    Ref<Value> construct() const override;
};

struct StructType : Type
{
    Array<Ref<LayerType>> layers;
    Array<Ref<Variable>> fields;
    Array<Ref<Method>> methods;

    Ref<Value> construct() const override;
};

// cmplx types
struct ArrayType : Type
{
    Ref<Type> type;

    ArrayType() { name = String( op_array_opn ) + String( op_array_cls ); }

    Ref<Value> construct() const override;
};

struct RangeType : Type
{
    RangeType() { name = op_range; }

    Ref<Value> construct() const override;
};

struct RefType : Type
{
    enum struct Kind
    {
        LET = 0,
        VAR,
    };

    Ref<Type> type;
    Kind kind = Kind::LET;

    Ref<Value> construct() const override;
};
}
