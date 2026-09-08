#include "dawn.h"

using namespace dawn; // Not in a header file, it's fine.

#ifdef _DEBUG

static constexpr bool DEBUG_TESTS = false;

struct Stopwatch
{
    using Clock = ch::high_resolution_clock;
    const Clock::time_point start_time = Clock::now();

    ~Stopwatch() noexcept
    {
        const Clock::time_point end_time = Clock::now();
        const auto duration = ch::duration_cast<ch::microseconds>(end_time - start_time);
        print("Elapsed time: ", duration);
    }
};

int main(int argc, char** argv)
{
    Stopwatch stopwatch;
    Dawn dawn;

    String error;
    dawn.eval_source(Source::from_file(DEBUG_TESTS ? "examples/tests.dw" : "examples/dev.dw"), &error);
    if (!error.empty())
    {
        print(error);
        return -1;
    }

    ArrayValue args;
    for (int i = 0; i < argc; i++)
        args.data.emplace_back<String>(argv[i]);

    error.clear();
    const Value retval = dawn.call_function("main", {Value{&args, true}}, &error);
    if (!error.empty())
    {
        print(error);
        return -2;
    }
    return (int)dawn.engine.to_int(retval);
}

#else

static constexpr StringRef DEFAULT_INPUT = ".";
static constexpr StringRef DIR_CONFIG_FILENAME = "dawn.ini";

int main(int argc, char** argv)
{
    Dawn dawn;
    if (auto error = dawn.config.from_args(argv + 1, argc - 1))
    {
        print(error.value());
        return -1;
    }

    if (dawn.config.input_file.empty())
        dawn.config.input_file = DEFAULT_INPUT;

    if (fs::is_directory(dawn.config.input_file))
    {
        if (auto error = dawn.config.from_file(format(dawn.config.input_file, "/", DIR_CONFIG_FILENAME)))
        {
            print(error.value());
            return -2;
        }
    }
    else if (fs::path{dawn.config.input_file}.filename().string() == DIR_CONFIG_FILENAME)
    {
        if (auto error = dawn.config.from_file(dawn.config.input_file))
        {
            print(error.value());
            return -3;
        }
    }

    try
    {
        const Source source = Source::from_file(dawn.config.input_file);
        String error;
        dawn.eval_source(source, &error);
        if (!error.empty())
        {
            print(error);
            return -5;
        }
    }
    catch (String const& error)
    {
        print(error);
        return -4;
    }

    ArrayValue args;
    for (String const& arg : dawn.config.args_to_pass)
        args.data.emplace_back(arg);

    String error;
    const Value retval = dawn.call_func("main", {Value{&args, true}}, &error);
    if (!error.empty())
    {
        print(error);
        return -6;
    }
    return (int)retval.to_int(dawn.engine);
}

#endif
