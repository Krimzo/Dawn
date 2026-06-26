#pragma once

#include "util.h"
#include "lexer.h"
#include "parser.h"
#include "optimizer.h"
#include "engine.h"


namespace dawn
{
struct Flags
{
    static constexpr StringRef _PREFIX = "-";
    static constexpr StringRef _CONFIG_MAIN_FILE = "main";
    static constexpr StringRef _CONFIG_ARGS_TO_PASS = "args";
    static constexpr StringRef DISABLE_OPTIMIZATIONS = "disopt";
};

struct Config
{
    String input_file;
    Vector<String> args_to_pass;
    StringMap<Bool> flags = {
        { (String) Flags::DISABLE_OPTIMIZATIONS, false }
    };

    Opt<String> from_args( char const* const* args, int count ) noexcept;
    Opt<String> from_file( StringRef const& path ) noexcept;
    Bool flag_status( StringRef const& flag ) const;
};

struct Dawn
{
    Config config{};
    StringSet imports;
    Lexer lexer;
    Parser parser;
    Optimizer optimizer;
    Engine engine;

    Opt<String> eval( Source const& source ) noexcept;

    void bind_func( StringRef const& name, Bool is_ctime, CFunction cfunc ) noexcept;
    Opt<String> call_func( StringRef const& name ) noexcept;
    Opt<String> call_func( StringRef const& name, Value* retval ) noexcept;
    Opt<String> call_func( StringRef const& name, std::initializer_list<Value> const& args, Value* retval = nullptr ) noexcept;
    Opt<String> call_func( StringRef const& name, Value* args, Int arg_count, Value* retval = nullptr ) noexcept;

    void add_var( Location const& location, VarType const& type, StringRef const& name, Value const& value ) noexcept;
    Value* get_var( StringRef const& name ) noexcept;
};
}
