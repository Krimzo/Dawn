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
};

struct IntType : Type
{
    IntType() { name = tp_int; }
};

struct FloatType : Type
{
    FloatType() { name = tp_float; }
};

struct CharType : Type
{
    CharType() { name = tp_char; }
};

struct StringType : Type
{
    StringType() { name = tp_string; }
};

struct EnumType : Type
{
    Ref<Type> type;
    Map<String, Ref<Value>> values;
};

struct LayerType : Type
{
    Map<String, Method> methods;
};

struct StructType : Type
{
    Map<String, Ref<Type>> fields_public;
    Map<String, Ref<Method>> methods_public;
    Map<String, Ref<Type>> fields_internal;
    Map<String, Ref<Method>> methods_internal;
};

// cmplx types
struct ArrayType : Type
{
    Ref<Type> type;

    ArrayType() { name = String( op_array_opn ) + String( op_array_cls ); }
};

struct RangeType : Type
{
    RangeType() { name = op_range; }
};

struct RefType : Type
{
    Ref<Type> type;
};

struct LetRefType : RefType
{
    LetRefType() { name = String( kw_let ) + String( op_ref ); }
};

struct VarRefType : RefType
{
    VarRefType() { name = String( kw_var ) + String( op_ref ); }
};
}
