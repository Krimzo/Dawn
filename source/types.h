#pragma once

#include "valnode.h"


namespace dawn
{
struct Variable
{
    enum struct Kind
    {
        LET = 0,
        VAR,
        REF,
    };

    String name;
    Kind kind = Kind::LET;
    Ref<Node> expr;
};

struct Scope : Node
{
    Array<Ref<Node>> instr;
};

struct Function
{
    using CppFunc = Func<Ref<Value>( Array<Ref<Value>> const& )>;

    String name;
    Array<Variable> args;
    Variant<Scope, CppFunc> body;
};

struct Operator : Function
{
};

struct Method : Function
{
    Bool is_var = false;
};

struct Enum
{
    String name;
    Array<Pair<String, Ref<Value>>> keys;
};

struct Layer
{
    String name;
    Array<Method> methods;
};

struct Struct
{
    String name;
    Array<Ref<Layer>> layers;
    Array<Ref<Variable>> fields;
    Array<Ref<Method>> methods;
};
}
