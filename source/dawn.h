#pragma once

#include "util.h"
#include "lexer.h"
#include "parser.h"
#include "engine.h"


namespace dawn
{
struct Dawn
{
    Lexer lexer;
    Parser parser;
    Engine engine;

    Dawn() noexcept;

    Opt<String> eval( StringRef const& source ) noexcept;
    Opt<String> eval( StringRef const& source, Set<String>& imports ) noexcept;

    Opt<String> eval_file( StringRef const& path ) noexcept;
    Opt<String> eval_file( StringRef const& path, Set<String>& imports ) noexcept;

    void bind_func( StringRef const& name, Function::CppFunc cpp_func ) noexcept;
    Opt<String> call_func( String const& name ) noexcept;
    Opt<String> call_func( String const& name, ValueRef& retval ) noexcept;
    Opt<String> call_func( String const& name, Array<ValueRef> const& args ) noexcept;
    Opt<String> call_func( String const& name, Array<ValueRef> const& args, ValueRef& retval ) noexcept;

    void add_var( VariableKind kind, String const& name, ValueRef const& value ) noexcept;
    ValueRef* get_var( String const& name ) noexcept;
};
}
