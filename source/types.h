#pragma once

#include "valnode.h"


namespace dawn
{
enum struct VariableKind
{
    LET = 0,
    VAR,
    REF,
};

struct Variable
{
    String name;
    VariableKind kind = VariableKind::LET;
    Ref<Node> expr;
};

struct Scope : Node
{
    Array<Ref<Node>> instr;
};

struct Function
{
    using CppFunc = Func<RawValue( Array<ValueBox> const& )>;

    String name;
    Array<Variable> args;
    Variant<Scope, CppFunc> body;
};

struct Enum
{
    String name;
    Array<Pair<String, RawValue>> keys;
};

struct Struct
{
    String name;
    List<Variable> fields;
    Array<Function> methods;

    Function const* get_method( StringRef const& name ) const;
};
}
