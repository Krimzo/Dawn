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
    StringMap<Bool> flags = {{(String)Flags::DISABLE_OPTIMIZATIONS, false}};

    Opt<String> from_args(char const* const* args, int count) noexcept;
    Opt<String> from_file(StringRef path) noexcept;
    Bool flag_status(StringRef flag) const;
};

struct Dawn
{
    Config config{};
    StringSet imports;
    Lexer lexer;
    Parser parser;
    Optimizer optimizer;
    Engine engine;

    void eval_source(Source const& source, String* out_error = nullptr) noexcept;
    Value eval_scope(StringRef scope_src, String* out_error = nullptr) noexcept;
    Value eval_expr(StringRef expr_src, String* out_error = nullptr) noexcept;

    void bind_func(ID id, Bool is_ctime, CFunction cfunc) noexcept;
    Value call_func(ID id, String* out_error = nullptr) noexcept;
    Value call_func(ID id, std::initializer_list<Value> const& args, String* out_error = nullptr) noexcept;
    Value call_func(ID id, Value* args, Int arg_count, String* out_error = nullptr) noexcept;

    void add_var(VarType const& type, ID id, Value const& value) noexcept;
    Value* get_var(ID id) noexcept;
};
} // namespace dawn
