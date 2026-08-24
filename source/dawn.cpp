#include "dawn.h"

dawn::Opt<dawn::String> dawn::Config::from_args(char const* const* args, int count) noexcept
{
    for (int i = 0; i < count; i++)
    {
        const std::string_view arg = args[i];
        if (arg.starts_with(Flags::_PREFIX))
        {
            if (arg.size() < Flags::_PREFIX.size() + 1)
                return format("invalid argument: ", arg);
            const std::string_view arg_value = arg.substr(Flags::_PREFIX.size());
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

dawn::Opt<dawn::String> dawn::Config::from_file(StringRef const& path) noexcept
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

dawn::Bool dawn::Config::flag_status(StringRef const& flag) const
{
    const auto it = flags.find(flag);
    if (it == flags.end())
        return false;
    return it->second;
}

dawn::Opt<dawn::String> dawn::Dawn::eval(Source const& source) noexcept
{
    try
    {
        if (source.path)
        {
            if (imports.contains(*source.path))
                return std::nullopt;
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
            if (auto error = eval(Source::from_file(path)))
                return error;
        }

        if (!config.flag_status(Flags::DISABLE_OPTIMIZATIONS))
            optimizer.optimize(module);
        engine.load_mod(module);
    }
    catch (String const& msg)
    {
        return msg;
    }
    return std::nullopt;
}

void dawn::Dawn::bind_func(ID id, Bool is_ctime, CFunction cfunc) noexcept
{
    engine.bind_func(id, is_ctime, std::move(cfunc));
}

dawn::Opt<dawn::String> dawn::Dawn::call_func(ID id) noexcept
{
    return call_func(id, nullptr, 0, nullptr);
}

dawn::Opt<dawn::String> dawn::Dawn::call_func(ID id, Value* retval) noexcept
{
    return call_func(id, nullptr, 0, retval);
}

dawn::Opt<dawn::String> dawn::Dawn::call_func(ID id, std::initializer_list<Value> const& args, Value* retval) noexcept
{
    return call_func(id, (Value*)args.begin(), (Int)args.size(), retval);
}

dawn::Opt<dawn::String> dawn::Dawn::call_func(ID id, Value* args, Int arg_count, Value* retval) noexcept
{
    try
    {
        if (retval)
            *retval = engine.call_func(id, args, arg_count);
        else
            engine.call_func(id, args, arg_count);
    }
    catch (String const& msg)
    {
        return msg;
    }
    catch (Value const& err)
    {
        return dawn::format("Uncaught error: ", err.to_string(engine));
    }
    return std::nullopt;
}

void dawn::Dawn::add_var(VarType const& type, ID id, Value const& value) noexcept
{
    engine.add_var(Location{}, type, id, value);
}

dawn::Value* dawn::Dawn::get_var(ID id) noexcept
{
    return engine.get_var(id);
}
