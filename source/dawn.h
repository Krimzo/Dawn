#pragma once

#include "util.h"
#include "lexer.h"
#include "parser.h"
#include "optimizer.h"
#include "engine.h"


namespace dawn
{
struct Dawn
{
    Lexer lexer;
    Parser parser;
    Optimizer optimizer;
    Engine engine;

    Opt<String> eval( Source const& source ) noexcept;
    Opt<String> eval( Source const& source, StringSet& imported ) noexcept;

    void bind_func( StringRef const& name, Bool is_ctime, CFunction cfunc ) noexcept;
    Opt<String> call_func( StringRef const& name ) noexcept;
    Opt<String> call_func( StringRef const& name, Value* retval ) noexcept;
    Opt<String> call_func( StringRef const& name, std::initializer_list<Value> const& args, Value* retval = nullptr ) noexcept;
    Opt<String> call_func( StringRef const& name, Value* args, Int arg_count, Value* retval = nullptr ) noexcept;

    void add_gvar( StringRef const& name, Value const& value ) noexcept;
    Value* get_gvar( StringRef const& name ) noexcept;
};
}
