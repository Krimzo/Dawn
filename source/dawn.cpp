#include "dawn.h"

dawn::Opt<dawn::String> dawn::Config::from_args(char const* const* args, int count) noexcept
{
    for (int i = 0; i < count; i++)
    {
        const StringRef arg = args[i];
        if (arg.starts_with(Flags::_PREFIX))
        {
            if (arg.size() < Flags::_PREFIX.size() + 1)
                return format("invalid argument: ", arg);
            const StringRef arg_value = arg.substr(Flags::_PREFIX.size());
            const auto it = flags.find(arg_value);
            if (it != flags.end())
                it->second = true;
            else
                args_to_pass.emplace_back(arg);
        }
        else if (input_file.empty())
            input_file = arg;
        else
            args_to_pass.emplace_back(arg);
    }
    return std::nullopt;
}

dawn::Opt<dawn::String> dawn::Config::from_file(StringRef path) noexcept
{
    const Opt<String> file_data = read_file(path);
    if (!file_data)
        return format("failed to read file: ", path);
    Bool has_main = false;
    for (String const& line : split(*file_data, "\n"))
    {
        const Vector<String> parts = split(line, "=");
        if (parts.empty())
            continue;
        if (parts.size() != 2)
            return format("invalid line: ", line);
        if (parts[0] == Flags::_CONFIG_MAIN_FILE)
        {
            std::error_code error;
            const fs::path abs_path = fs::path{path}.parent_path() / parts[1];
            input_file = fs::canonical(abs_path, error).generic_string();
            if (error)
                return format("main file ", abs_path, " does not exist");
            has_main = true;
        }
        else if (parts[0] == Flags::_CONFIG_ARGS_TO_PASS)
        {
            for (auto& arg : split(parts[1], " "))
            {
                if (arg.empty())
                    continue;
                args_to_pass.emplace_back(arg);
            }
        }
        else
        {
            const auto it = flags.find(parts[0]);
            if (it == flags.end())
                return format("unknown config flag: ", parts[0]);
            it->second = (parts[1] == "true");
        }
    }
    if (!has_main)
        return format("missing config flag: ", Flags::_CONFIG_MAIN_FILE);
    return std::nullopt;
}

dawn::Bool dawn::Config::flag_status(StringRef flag) const
{
    const auto it = flags.find(flag);
    if (it == flags.end())
        return false;
    return it->second;
}

void dawn::Dawn::eval_source(Source const& source, String* out_error) noexcept
{
    try
    {
        if (source.path)
        {
            if (imports.contains(*source.path))
                return;
            imports.insert(*source.path);
        }

        Vector<Token> tokens;
        lexer.tokenize(source, tokens);

        Module module;
        parser.parse(tokens, module);

        for (String path : module.imports)
        {
            if (fs::path{path}.is_relative())
            {
                if (!source.path)
                    throw String("import can only be used inside dawn files");
                path = fs::path{*source.path}.parent_path().string() + "/" + path;
            }
            String error;
            eval_source(Source::from_file(path), &error);
            if (!error.empty())
            {
                if (out_error)
                    *out_error = error;
                return;
            }
        }

        if (!config.flag_status(Flags::DISABLE_OPTIMIZATIONS))
            optimizer.optimize(module);
        engine.load_module(module);
    }
    catch (String const& msg)
    {
        if (out_error)
            *out_error = msg;
    }
}

dawn::Value dawn::Dawn::eval_scope(StringRef scope_src, String* out_error) noexcept
{
    Opt<Value> retval;
    try
    {
        Vector<Token> tokens;
        tokens.push_back(Token{.type = TokenType::OPERATOR, .value = (String)op_scope_opn});
        lexer.tokenize(Source::from_text(scope_src), tokens);
        tokens.push_back(Token{.type = TokenType::OPERATOR, .value = (String)op_scope_cls});

        Scope scope;
        TokenIterator it{tokens.data(), tokens.data() + tokens.size()};
        parser.parse_scope(it, scope);

        engine.handle_scope(scope, retval, nullptr, nullptr);
    }
    catch (String const& msg)
    {
        if (out_error)
            *out_error = msg;
    }
    return retval.value_or({});
}

dawn::Value dawn::Dawn::eval_expression(StringRef expr_src, String* out_error) noexcept
{
    try
    {
        Vector<Token> tokens;
        lexer.tokenize(Source::from_text(expr_src), tokens);

        Node expr;
        TokenIterator it{tokens.data(), tokens.data() + tokens.size()};
        parser.parse_expression(ExtractType::NEW_LINE_EXP_END, it, expr);

        return engine.handle_expression(expr);
    }
    catch (String const& msg)
    {
        if (out_error)
            *out_error = msg;
    }
    return {};
}

void dawn::Dawn::bind_function(ID id, Bool is_ctime, CFunction cfunc) noexcept
{
    engine.bind_function(id, is_ctime, std::move(cfunc));
}

dawn::Value dawn::Dawn::call_function(ID id, String* out_error) noexcept
{
    return call_function(id, nullptr, 0, out_error);
}

dawn::Value dawn::Dawn::call_function(ID id, std::initializer_list<Value> const& args, String* out_error) noexcept
{
    return call_function(id, (Value*)args.begin(), (Int)args.size(), out_error);
}

dawn::Value dawn::Dawn::call_function(ID id, Value* args, Int arg_count, String* out_error) noexcept
{
    try
    {
        return engine.call_function(id, args, arg_count);
    }
    catch (String const& msg)
    {
        if (out_error)
            *out_error = msg;
    }
    catch (Value const& err)
    {
        // if (out_error)
        //     *out_error = dawn::format("Uncaught error: ", err.to_string(engine));
    }
    return {};
}

void dawn::Dawn::add_variable(VarType const& type, ID id, Value const& value) noexcept
{
    engine.add_variable(Location{}, type, id, value);
}

dawn::Value* dawn::Dawn::get_variable(ID id) noexcept
{
    return engine.get_variable(id);
}
