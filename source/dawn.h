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

    void bind_func( String const& name, CFunction cfunc ) noexcept;
    Opt<String> call_func( String const& name ) noexcept;
    Opt<String> call_func( String const& name, Value* retval ) noexcept;
    Opt<String> call_func( String const& name, std::initializer_list<Value> const& args, Value* retval = nullptr ) noexcept;
    Opt<String> call_func( String const& name, Value* args, Int arg_count, Value* retval = nullptr ) noexcept;

    void add_var( VariableKind kind, String const& name, Value const& value ) noexcept;
    Value* get_var( String const& name ) noexcept;
};
}
