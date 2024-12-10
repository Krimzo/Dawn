#pragma once

#include "util.h"
#include "lexer.h"
#include "parser.h"
#include "engine.h"


namespace dawn
{
struct Dawn
{
    Dawn();

    Opt<String> eval( StringRef const& source );
    Opt<String> eval_file( StringRef const& path );

    void bind_func( String const& name, Function::CppFunc cpp_func );
    Opt<EngineError> call_func( String const& name, Array<Ref<Node>> const& args, ValueBox& retval );

    Opt<EngineError> add_var( Variable const& var );
    void add_var( Variable const& var, ValueBox const& value );
    ValueBox* get_var( String const& name );

private:
    Lexer m_lexer;
    Parser m_parser;
    Engine m_engine;
};
}
