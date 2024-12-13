#pragma once

#include "util.h"
#include "lexer.h"
#include "parser.h"
#include "engine.h"


namespace dawn
{
struct Dawn
{
    Dawn() noexcept;

    Opt<String> eval( StringRef const& source ) noexcept;
    Opt<String> eval_file( StringRef const& path ) noexcept;

    void bind_func( StringRef const& name, Function::CppFunc cpp_func ) noexcept;
    Opt<String> call_func( String const& name ) noexcept;
    Opt<String> call_func( String const& name, ValueRef& retval ) noexcept;
    Opt<String> call_func( String const& name, Array<Node>& args ) noexcept;
    Opt<String> call_func( String const& name, Array<Node>& args, ValueRef& retval ) noexcept;

    Opt<String> add_var( Variable& var ) noexcept;
    void add_var( VariableKind kind, String const& name, ValueRef const& value ) noexcept;
    ValueRef* get_var( String const& name ) noexcept;

private:
    Lexer m_lexer;
    Parser m_parser;
    Engine m_engine;
};
}
