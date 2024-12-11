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

    void bind_func( String const& name, Function::CppFunc cpp_func ) noexcept;
    Opt<String> call_func( String const& name, Array<Node> const& args, ValueBox& retval ) noexcept;

    Opt<String> add_var( Variable const& var ) noexcept;
    void add_var( Variable const& var, ValueBox const& value ) noexcept;
    ValueBox* get_var( String const& name ) noexcept;

private:
    Lexer m_lexer;
    Parser m_parser;
    Engine m_engine;
};
}
