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

struct Enum
{
    String name;
    Array<Pair<String, Ref<Value>>> keys;
};

struct Struct
{
    String name;
    Array<Ref<Variable>> fields;
    Array<Ref<Function>> methods;
};
}
