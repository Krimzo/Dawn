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

    Opt<String> eval( StringRef const& source ) noexcept;
    Opt<String> eval( StringRef const& source, Opt<String> const& parent_path, Set<String>& imported ) noexcept;

    Opt<String> eval_file( StringRef const& path ) noexcept;
    Opt<String> eval_file( StringRef const& path, Set<String>& imported ) noexcept;

    void bind_func( String const& name, Function::CppFunc cpp_func ) noexcept;
    Opt<String> call_func( String const& name ) noexcept;
    Opt<String> call_func( String const& name, ValueRef& retval ) noexcept;
    Opt<String> call_func( String const& name, std::initializer_list<ValueRef> const& args ) noexcept;
    Opt<String> call_func( String const& name, std::initializer_list<ValueRef> const& args, ValueRef& retval ) noexcept;

    void add_obj( VariableKind kind, String const& name, ValueRef const& value ) noexcept;
    ValueRef* get_obj( String const& name ) noexcept;
};
}
