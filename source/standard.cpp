#include "engine.h"

static thread_local std::mt19937_64 RAND_ENGINE = [] {
    std::random_device device{};
    std::srand(device());
    return std::mt19937_64{device()};
}();

void dawn::Engine::load_standard_operators()
{
    // op add
    bind_operator(id_void, OperatorType::ADD, id_int, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{+right.as_int(), location};
                  });

    bind_operator(id_void, OperatorType::ADD, id_float, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{+right.as_float(), location};
                  });

    bind_operator(id_int, OperatorType::ADD, id_int, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_int() + right.as_int(), location};
                  });

    bind_operator(id_int, OperatorType::ADD, id_float, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_int() + right.as_float(), location};
                  });

    bind_operator(id_float, OperatorType::ADD, id_int, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_float() + right.as_int(), location};
                  });

    bind_operator(id_float, OperatorType::ADD, id_float, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_float() + right.as_float(), location};
                  });

    bind_operator(id_string, OperatorType::ADD, id_string, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_string() + right.as_string(), location};
                  });

    bind_operator(id_array, OperatorType::ADD, id_array, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      ArrayValue result;
                      result.data.insert(result.data.end(), left.as_array().data.begin(), left.as_array().data.end());
                      result.data.insert(result.data.end(), right.as_array().data.begin(), right.as_array().data.end());
                      return Value{result, location};
                  });

    // op sub
    bind_operator(id_void, OperatorType::SUB, id_int, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{-right.as_int(), location};
                  });

    bind_operator(id_void, OperatorType::SUB, id_float, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{-right.as_float(), location};
                  });

    bind_operator(id_int, OperatorType::SUB, id_int, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_int() - right.as_int(), location};
                  });

    bind_operator(id_int, OperatorType::SUB, id_float, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_int() - right.as_float(), location};
                  });

    bind_operator(id_float, OperatorType::SUB, id_int, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_float() - right.as_int(), location};
                  });

    bind_operator(id_float, OperatorType::SUB, id_float, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_float() - right.as_float(), location};
                  });

    // op mul
    bind_operator(id_int, OperatorType::MUL, id_int, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_int() * right.as_int(), location};
                  });

    bind_operator(id_int, OperatorType::MUL, id_float, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_int() * right.as_float(), location};
                  });

    bind_operator(id_float, OperatorType::MUL, id_int, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_float() * right.as_int(), location};
                  });

    bind_operator(id_float, OperatorType::MUL, id_float, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_float() * right.as_float(), location};
                  });

    // op div
    bind_operator(id_int, OperatorType::DIV, id_int, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_int() / right.as_int(), location};
                  });

    bind_operator(id_int, OperatorType::DIV, id_float, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_int() / right.as_float(), location};
                  });

    bind_operator(id_float, OperatorType::DIV, id_int, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_float() / right.as_int(), location};
                  });

    bind_operator(id_float, OperatorType::DIV, id_float, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_float() / right.as_float(), location};
                  });

    // op pow
    bind_operator(id_int, OperatorType::POW, id_int, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{(Int)std::pow(left.as_int(), right.as_int()), location};
                  });

    bind_operator(id_int, OperatorType::POW, id_float, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{std::pow(left.as_int(), right.as_float()), location};
                  });

    bind_operator(id_float, OperatorType::POW, id_int, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{std::pow(left.as_float(), right.as_int()), location};
                  });

    bind_operator(id_float, OperatorType::POW, id_float, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{std::pow(left.as_float(), right.as_float()), location};
                  });

    // op mod
    bind_operator(id_int, OperatorType::MOD, id_int, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_int() % right.as_int(), location};
                  });

    bind_operator(id_int, OperatorType::MOD, id_float, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{dawn_mod((Float)left.as_int(), right.as_float()), location};
                  });

    bind_operator(id_float, OperatorType::MOD, id_int, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{dawn_mod(left.as_float(), (Float)right.as_int()), location};
                  });

    bind_operator(id_float, OperatorType::MOD, id_float, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{dawn_mod(left.as_float(), right.as_float()), location};
                  });

    // op equals
    bind_operator(id_void, OperatorType::EQ, id_void, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{Bool{true}, location};
                  });

    bind_operator(id_bool, OperatorType::EQ, id_bool, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_bool() == right.as_bool(), location};
                  });

    bind_operator(id_int, OperatorType::EQ, id_int, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_int() == right.as_int(), location};
                  });

    bind_operator(id_int, OperatorType::EQ, id_float, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_int() == right.as_float(), location};
                  });

    bind_operator(id_float, OperatorType::EQ, id_float, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_float() == right.as_float(), location};
                  });

    bind_operator(id_float, OperatorType::EQ, id_int, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_float() == right.as_int(), location};
                  });

    bind_operator(id_char, OperatorType::EQ, id_char, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_char() == right.as_char(), location};
                  });

    bind_operator(id_string, OperatorType::EQ, id_string, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_string() == right.as_string(), location};
                  });

    // op not equals
    bind_operator(id_void, OperatorType::NOT_EQ, id_void, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{Int{false}, location};
                  });

    bind_operator(id_bool, OperatorType::NOT_EQ, id_bool, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_bool() != right.as_bool(), location};
                  });

    bind_operator(id_int, OperatorType::NOT_EQ, id_int, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_int() != right.as_int(), location};
                  });

    bind_operator(id_int, OperatorType::NOT_EQ, id_float, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_int() != right.as_float(), location};
                  });

    bind_operator(id_float, OperatorType::NOT_EQ, id_float, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_float() != right.as_float(), location};
                  });

    bind_operator(id_float, OperatorType::NOT_EQ, id_int, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_float() != right.as_int(), location};
                  });

    bind_operator(id_char, OperatorType::NOT_EQ, id_char, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_char() != right.as_char(), location};
                  });

    bind_operator(id_string, OperatorType::NOT_EQ, id_string, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_string() != right.as_string(), location};
                  });

    // op less
    bind_operator(id_int, OperatorType::LESS, id_int, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_int() < right.as_int(), location};
                  });

    bind_operator(id_int, OperatorType::LESS, id_float, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_int() < right.as_float(), location};
                  });

    bind_operator(id_float, OperatorType::LESS, id_float, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_float() < right.as_float(), location};
                  });

    bind_operator(id_float, OperatorType::LESS, id_int, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_float() < right.as_int(), location};
                  });

    // op great
    bind_operator(id_int, OperatorType::GREAT, id_int, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_int() > right.as_int(), location};
                  });

    bind_operator(id_int, OperatorType::GREAT, id_float, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_int() > right.as_float(), location};
                  });

    bind_operator(id_float, OperatorType::GREAT, id_float, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_float() > right.as_float(), location};
                  });

    bind_operator(id_float, OperatorType::GREAT, id_int, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_float() > right.as_int(), location};
                  });

    // op less equals
    bind_operator(id_int, OperatorType::LESS_EQ, id_int, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_int() <= right.as_int(), location};
                  });

    bind_operator(id_int, OperatorType::LESS_EQ, id_float, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_int() <= right.as_float(), location};
                  });

    bind_operator(id_float, OperatorType::LESS_EQ, id_float, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_float() <= right.as_float(), location};
                  });

    bind_operator(id_float, OperatorType::LESS_EQ, id_int, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_float() <= right.as_int(), location};
                  });

    // op great equals
    bind_operator(id_int, OperatorType::GREAT_EQ, id_int, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_int() >= right.as_int(), location};
                  });

    bind_operator(id_int, OperatorType::GREAT_EQ, id_float, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_int() >= right.as_float(), location};
                  });

    bind_operator(id_float, OperatorType::GREAT_EQ, id_float, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_float() >= right.as_float(), location};
                  });

    bind_operator(id_float, OperatorType::GREAT_EQ, id_int, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_float() >= right.as_int(), location};
                  });

    // op not
    bind_operator(id_void, OperatorType::NOT, id_bool, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{!right.as_bool(), location};
                  });

    // op and
    bind_operator(id_bool, OperatorType::AND, id_bool, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_bool() && right.as_bool(), location};
                  });

    // op or
    bind_operator(id_bool, OperatorType::OR, id_bool, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{left.as_bool() || right.as_bool(), location};
                  });

    // op range
    bind_operator(id_int, OperatorType::RANGE, id_int, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{RangeValue{.start = left.as_int(), .end = right.as_int(), .inclusive = false},
                                   location};
                  });

    bind_operator(id_void, OperatorType::RANGE, id_int, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto const& right = args[1];
                      return Value{RangeValue{.start = 0, .end = right.as_int(), .inclusive = false}, location};
                  });

    // op range_incl
    bind_operator(id_int, OperatorType::RANGE_INCL, id_int, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto& left = args[0];
                      auto& right = args[1];
                      return Value{RangeValue{.start = left.as_int(), .end = right.as_int(), .inclusive = true},
                                   location};
                  });

    bind_operator(id_void, OperatorType::RANGE_INCL, id_int, true,
                  [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
                      auto const& right = args[1];
                      return Value{RangeValue{.start = 0, .end = right.as_int(), .inclusive = true}, location};
                  });
}

void dawn::Engine::load_standard_functions()
{
    static const Lexer _lexer{};
    static constexpr Parser _parser{};

    // builtin
    bind_function(id_void, true, [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
        if (arg_count == 0)
            return Value{};
        else if (arg_count == 1)
            return Value{};
        else
            ENGINE_PANIC(location, id_void, " constructor supports only 0 or 1 arguments");
    });
    bind_function(id_bool, true, [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
        if (arg_count == 0)
            return Value{Bool{}, location};
        else if (arg_count == 1)
            return Value{engine.to_bool(args[0]), location};
        else
            ENGINE_PANIC(location, id_bool, " constructor supports only 0 or 1 arguments");
    });
    bind_function(id_int, true, [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
        if (arg_count == 0)
            return Value{Int{}, location};
        else if (arg_count == 1)
            return Value{engine.to_int(args[0]), location};
        else
            ENGINE_PANIC(location, id_int, " constructor supports only 0 or 1 arguments");
    });
    bind_function(id_float, true, [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
        if (arg_count == 0)
            return Value{Float{}, location};
        else if (arg_count == 1)
            return Value{engine.to_float(args[0]), location};
        else
            ENGINE_PANIC(location, id_float, " constructor supports only 0 or 1 arguments");
    });
    bind_function(id_char, true, [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
        if (arg_count == 0)
            return Value{Char{}, location};
        else if (arg_count == 1)
            return Value{engine.to_char(args[0]), location};
        else
            ENGINE_PANIC(location, id_char, " constructor supports only 0 or 1 arguments");
    });
    bind_function(id_string, true, [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
        if (arg_count == 0)
            return Value{String{}, location};
        else if (arg_count == 1)
            return Value{engine.to_string(args[0]), location};
        else
            ENGINE_PANIC(location, id_string, " constructor supports only 0 or 1 arguments");
    });
    bind_function(id_range, true, [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
        if (arg_count == 0)
            return Value{RangeValue{}, location};
        else if (arg_count == 1)
            return Value{engine.to_range(args[0]), location};
        else
            ENGINE_PANIC(location, id_range, " constructor supports only 0 or 1 arguments");
    });
    bind_function(id_function, true, [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
        if (arg_count == 0)
            return Value{FunctionValue{}, location};
        else if (arg_count == 1)
            return Value{engine.to_function(args[0]), location};
        else
            ENGINE_PANIC(location, id_function, " constructor supports only 0 or 1 arguments");
    });
    bind_function(id_array, true, [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
        if (arg_count == 0)
            return Value{ArrayValue{}, location};
        else if (arg_count == 1)
            return Value{engine.to_array(args[0]), location};
        else
            ENGINE_PANIC(location, id_array, " constructor supports only 0 or 1 arguments");
    });

    // dawn
    bind_function("lex", true, [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
        if (arg_count != 1)
            ENGINE_PANIC(location, "lex expected 1 argument, but got ", arg_count);
        Vector<Token> tokens;
        _lexer.tokenize(Source::from_text(args[0].as_string()), tokens);
        ArrayValue array;
        for (auto& token : tokens)
            array.data.emplace_back(token.any_value(), token.location);
        return Value{array, location};
    });

    bind_function("eval", true, [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
        if (arg_count != 1)
            ENGINE_PANIC(location, "eval expected 1 argument, but got ", arg_count);
        Vector<Token> tokens;
        _lexer.tokenize(Source::from_text(args[0].as_string()), tokens);
        TokenIterator it{tokens.data(), tokens.data() + tokens.size()};
        Node node;
        _parser.parse_expression(ExtractType::WHOLE, it, node);
        return engine.handle_expression(node);
    });

    // type info
    bind_function("typeid", true, [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
        if (arg_count == 1)
            return Value{(Int)args[0].type_id().integer(), location};
        else
            ENGINE_PANIC(location, "typeid expected 1 argument, but got ", arg_count);
    });

    bind_function("typename", true, [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
        if (arg_count == 1)
            return Value{args[0].type_id().string(), location};
        else
            ENGINE_PANIC(location, "typename expected 1 argument, but got ", arg_count);
    });

    bind_function("is_const", true, [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
        if (arg_count == 1)
            return Value{(Bool)args[0].is_const(), location};
        else
            ENGINE_PANIC(location, "is_const expected 1 argument, but got ", arg_count);
    });

    // system
    bind_function("exit", false, [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
        if (arg_count == 1)
            std::exit((int)args[0].as_int());
        else
            ENGINE_PANIC(location, "exit() expects 1 argument, but got ", arg_count);
        return {};
    });

    // util
    bind_function("format", true, [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
        StringStream stream;
        for (Int i = 0; i < arg_count; i++)
            stream << engine.to_string(args[i]);
        return Value{stream.str(), location};
    });

    bind_function("put", false, [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
        StringStream stream;
        for (Int i = 0; i < arg_count; i++)
            stream << engine.to_string(args[i]);
        put(stream.str());
        return Value{};
    });

    bind_function("print", false, [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
        StringStream stream;
        for (Int i = 0; i < arg_count; i++)
            stream << engine.to_string(args[i]);
        print(stream.str());
        return Value{};
    });

    bind_function("rand_int", false, [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
        if (arg_count == 1)
            return Value{Int(RAND_ENGINE() % args[0].as_int()), location};
        else
            ENGINE_PANIC(location, "rand_int() expects 1 argument, but got ", arg_count);
    });

    bind_function("rand_flt", false, [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
        if (arg_count == 0)
            return Value{((Float)RAND_ENGINE() / UINT64_MAX), location};
        else
            ENGINE_PANIC(location, "rand_flt() expects 0 arguments, but got ", arg_count);
    });

    // math
    bind_function("min", true, [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
        if (arg_count == 2)
        {
            if (args[0].type_id() == id_int)
                return Value{std::min(args[0].as_int(), args[1].as_int()), location};
            else if (args[0].type_id() == id_float)
                return Value{std::min(args[0].as_float(), args[1].as_float()), location};
            else
                ENGINE_PANIC(location, "min() expects an int or float");
        }
        else
            ENGINE_PANIC(location, "min() expects 2 arguments, but got ", arg_count);
    });

    bind_function("max", true, [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
        if (arg_count == 2)
        {
            if (args[0].type_id() == id_int)
                return Value{std::max(args[0].as_int(), args[1].as_int()), location};
            else if (args[0].type_id() == id_float)
                return Value{std::max(args[0].as_float(), args[1].as_float()), location};
            else
                ENGINE_PANIC(location, "max() expects an int or float");
        }
        else
            ENGINE_PANIC(location, "max() expects 2 arguments, but got ", arg_count);
    });

    bind_function("abs", true, [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
        if (arg_count == 1)
        {
            if (args[0].type_id() == id_int)
                return Value{std::abs(args[0].as_int()), location};
            else if (args[0].type_id() == id_float)
                return Value{std::abs(args[0].as_float()), location};
            else
                ENGINE_PANIC(location, "abs() expects an int or float");
        }
        else
            ENGINE_PANIC(location, "abs() expects 1 argument, but got ", arg_count);
    });

    bind_function("sqrt", true, [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
        if (arg_count == 1)
            return Value{std::sqrt(args[0].as_float()), location};
        else
            ENGINE_PANIC(location, "sqrt() expects 1 argument, but got ", arg_count);
    });

    bind_function("sin", true, [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
        if (arg_count == 1)
            return Value{std::sin(args[0].as_float()), location};
        else
            ENGINE_PANIC(location, "sin() expects 1 argument, but got ", arg_count);
    });

    bind_function("cos", true, [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
        if (arg_count == 1)
            return Value{std::cos(args[0].as_float()), location};
        else
            ENGINE_PANIC(location, "cos() expects 1 argument, but got ", arg_count);
    });
}

void dawn::Engine::load_standard_members()
{
    // string
    bind_method(id_string, "for_each", true, 1,
                [](Location location, Engine& engine, Value const& self, Value const* args) -> Value {
                    auto& func = args[0].as_function();
                    for (Char& c : self.as_string())
                    {
                        Value arg{&c, self.is_const(), location};
                        engine.handle_function(location, func, &arg, 1);
                    }
                    return Value{};
                });

    bind_field(id_string, "count", [](Location location, Engine& engine, Value const& self) -> Value {
        return Value{(Int)self.as_string().size(), location};
    });

    bind_method(id_string, "push", false, 1,
                [](Location location, Engine& engine, Value const& self, Value const* args) -> Value {
                    self.as_string().push_back(args[0].as_char());
                    return self;
                });

    bind_method(id_string, "pop", false, 0,
                [](Location location, Engine& engine, Value const& self, Value const* args) -> Value {
                    self.as_string().pop_back();
                    return self;
                });

    bind_method(id_string, "find", true, 1,
                [](Location location, Engine& engine, Value const& self, Value const* args) -> Value {
                    auto& self_str = self.as_string();
                    size_t index = self_str.find(args[0].as_string());
                    return Value{Int(index), location};
                });

    // range
    bind_field(id_range, "start", [](Location location, Engine& engine, Value const& self) -> Value {
        return Value{&self.as_range().start, self.is_const(), location};
    });

    bind_field(id_range, "end", [](Location location, Engine& engine, Value const& self) -> Value {
        return Value{&self.as_range().end, self.is_const(), location};
    });

    bind_field(id_range, "inclusive", [](Location location, Engine& engine, Value const& self) -> Value {
        return Value{&self.as_range().inclusive, self.is_const(), location};
    });

    // array
    bind_method(id_array, "for_each", true, 1,
                [](Location location, Engine& engine, Value const& self, Value const* args) -> Value {
                    auto& func = args[0].as_function();
                    for (Value const& v : self.as_array().data)
                        engine.handle_function(location, func, &v, 1);
                    return Value{};
                });

    bind_field(id_array, "count", [](Location location, Engine& engine, Value const& self) -> Value {
        return Value{(Int)self.as_array().data.size(), location};
    });

    bind_method(id_array, "push", false, 1,
                [](Location location, Engine& engine, Value const& self, Value const* args) -> Value {
                    self.as_array().data.emplace_back(args[0]).unlock_const();
                    return self;
                });

    bind_method(id_array, "pop", false, 0,
                [](Location location, Engine& engine, Value const& self, Value const* args) -> Value {
                    self.as_array().data.pop_back();
                    return self;
                });

    bind_method(id_array, "find", true, 1,
                [](Location location, Engine& engine, Value const& self, Value const* args) -> Value {
                    auto const& self_arr = self.as_array().data;
                    auto const& item = args[0];
                    for (Int i = 0; i < (Int)self_arr.size(); i++)
                    {
                        auto& element = self_arr[i];
                        if (element.type_id() == item.type_id() &&
                            engine.handle_operator(location, element, OperatorType::EQ, item).as_bool())
                            return Value{i, location};
                    }
                    return Value{Int(-1), location};
                });
}

void dawn::Engine::load_standard_casts()
{
    // to bool
    bind_cast(id_void, id_bool, true,
              [](Location location, Engine& engine, Value const& value) { return Value{Bool{}, location}; });
    bind_cast(id_bool, id_bool, true,
              [](Location location, Engine& engine, Value const& value) { return Value{value.as_bool(), location}; });
    bind_cast(id_int, id_bool, true, [](Location location, Engine& engine, Value const& value) {
        return Value{(Bool)value.as_int(), location};
    });
    bind_cast(id_float, id_bool, true, [](Location location, Engine& engine, Value const& value) {
        return Value{(Bool)value.as_float(), location};
    });
    bind_cast(id_char, id_bool, true, [](Location location, Engine& engine, Value const& value) {
        return Value{(Bool)value.as_char(), location};
    });

    // to int
    bind_cast(id_void, id_int, true,
              [](Location location, Engine& engine, Value const& value) { return Value{Int{}, location}; });
    bind_cast(id_bool, id_int, true, [](Location location, Engine& engine, Value const& value) {
        return Value{(Int)value.as_bool(), location};
    });
    bind_cast(id_int, id_int, true,
              [](Location location, Engine& engine, Value const& value) { return Value{value.as_int(), location}; });
    bind_cast(id_float, id_int, true, [](Location location, Engine& engine, Value const& value) {
        return Value{(Int)value.as_float(), location};
    });
    bind_cast(id_char, id_int, true, [](Location location, Engine& engine, Value const& value) {
        return Value{(Int)value.as_char(), location};
    });
    bind_cast(id_string, id_int, true, [](Location location, Engine& engine, Value const& value) {
        if (auto optres = parse_int(value.as_string()))
            return Value{*optres, location};
        throw Value{dawn::format("string \"", value.as_string(), "\" to int failed"), location};
    });

    // to float
    bind_cast(id_void, id_float, true,
              [](Location location, Engine& engine, Value const& value) { return Value{Float{}, location}; });
    bind_cast(id_bool, id_float, true, [](Location location, Engine& engine, Value const& value) {
        return Value{(Float)value.as_bool(), location};
    });
    bind_cast(id_int, id_float, true, [](Location location, Engine& engine, Value const& value) {
        return Value{(Float)value.as_int(), location};
    });
    bind_cast(id_float, id_float, true,
              [](Location location, Engine& engine, Value const& value) { return Value{value.as_float(), location}; });
    bind_cast(id_char, id_float, true, [](Location location, Engine& engine, Value const& value) {
        return Value{(Float)value.as_char(), location};
    });
    bind_cast(id_string, id_float, true, [](Location location, Engine& engine, Value const& value) {
        if (auto optres = parse_float(value.as_string()))
            return Value{*optres, location};
        throw Value{dawn::format("string \"", value.as_string(), "\" to float failed"), location};
    });

    // to char
    bind_cast(id_void, id_char, true,
              [](Location location, Engine& engine, Value const& value) { return Value{Char{}, location}; });
    bind_cast(id_bool, id_char, true, [](Location location, Engine& engine, Value const& value) {
        return Value{(Char)value.as_bool(), location};
    });
    bind_cast(id_int, id_char, true, [](Location location, Engine& engine, Value const& value) {
        return Value{(Char)value.as_int(), location};
    });
    bind_cast(id_float, id_char, true, [](Location location, Engine& engine, Value const& value) {
        return Value{(Char)value.as_float(), location};
    });
    bind_cast(id_char, id_char, true,
              [](Location location, Engine& engine, Value const& value) { return Value{value.as_char(), location}; });
    bind_cast(id_string, id_char, true, [](Location location, Engine& engine, Value const& value) {
        auto& str = value.as_string();
        if (str.empty())
            return Value{Char{}, location};
        return Value{str.front(), location};
    });

    // to string
    bind_cast(id_void, id_string, true,
              [](Location location, Engine& engine, Value const& value) { return Value{String{}, location}; });
    bind_cast(id_bool, id_string, true, [](Location location, Engine& engine, Value const& value) {
        return Value{String{value.as_bool() ? kw_true : kw_false}, location};
    });
    bind_cast(id_int, id_string, true, [](Location location, Engine& engine, Value const& value) {
        return Value{std::to_string(value.as_int()), location};
    });
    bind_cast(id_float, id_string, true, [](Location location, Engine& engine, Value const& value) {
        return Value{format(std::showpoint, value.as_float()), location};
    });
    bind_cast(id_char, id_string, true, [](Location location, Engine& engine, Value const& value) {
        return Value{String{value.as_char()}, location};
    });
    bind_cast(id_string, id_string, true,
              [](Location location, Engine& engine, Value const& value) { return Value{value.as_string(), location}; });
    bind_cast(id_range, id_string, true, [](Location location, Engine& engine, Value const& value) {
        auto& range_val = value.as_range();
        if (range_val.inclusive)
            return Value{format(op_array_opn, range_val.start, " -> ", range_val.end, op_array_cls), location};
        else
            return Value{format(op_array_opn, range_val.start, " -> ", range_val.end, op_expr_cls), location};
    });
    bind_cast(id_function, id_string, true, [](Location location, Engine& engine, Value const& value) {
        StringStream stream;
        auto& func = value.as_function();
        if (func.is_global())
        {
            auto& global = func.as_global();
            stream << global.id << op_expr_opn;
        }
        else if (func.is_method())
        {
            auto& method = func.as_method();
            stream << method.self->as_struct().parent_id << op_access << method.id << op_expr_opn;
        }
        else
            stream << "lambda" << op_lambda;
        if (auto* dfunc = func.dfunction())
        {
            if (!dfunc->args.empty())
            {
                for (Int i = 0; i < (Int)dfunc->args.size() - 1; i++)
                    stream << dfunc->args[i].type << ' ' << dfunc->args[i].id << op_split << ' ';
                stream << dfunc->args.back().type << ' ' << dfunc->args.back().id;
            }
        }
        stream << (func.is_lambda() ? op_lambda : op_expr_cls);
        return Value{stream.str(), location};
    });
    bind_cast(id_array, id_string, true, [](Location location, Engine& engine, Value const& value) {
        auto& array_val = value.as_array();
        if (array_val.data.empty())
            return Value{format(op_array_opn, op_array_cls), location};
        StringStream stream;
        stream << op_array_opn;
        for (Int i = 0; i < (Int)array_val.data.size() - 1; i++)
            stream << engine.to_string(array_val.data[i]) << op_split << ' ';
        stream << engine.to_string(array_val.data.back()) << op_array_cls;
        return Value{stream.str(), location};
    });

    // to range
    bind_cast(id_void, id_range, true,
              [](Location location, Engine& engine, Value const& value) { return Value{RangeValue{}, location}; });
    bind_cast(id_int, id_range, true, [](Location location, Engine& engine, Value const& value) {
        return Value{RangeValue{0, value.as_int()}, location};
    });
    bind_cast(id_range, id_range, true,
              [](Location location, Engine& engine, Value const& value) { return Value{value.as_range(), location}; });

    // to function
    bind_cast(id_function, id_function, true, [](Location location, Engine& engine, Value const& value) {
        return Value{value.as_function(), location};
    });

    // to array
    bind_cast(id_void, id_array, true,
              [](Location location, Engine& engine, Value const& value) { return Value{ArrayValue{}, location}; });
    bind_cast(id_string, id_array, true, [](Location location, Engine& engine, Value const& value) {
        auto& strval = value.as_string();
        ArrayValue result;
        result.data.reserve(strval.size());
        for (Char c : strval)
            result.data.emplace_back(c, location);
        return Value{result, location};
    });
    bind_cast(id_array, id_array, true,
              [](Location location, Engine& engine, Value const& value) { return Value{value.as_array(), location}; });
}

void dawn::Engine::load_enum_standards(Enum const& enu)
{
    auto& members = this->members.get_or_set(enu.id);
    auto& casts = this->casts.get_or_set(enu.id);
    members.clear();
    casts.clear();

    bind_field(enu.id, "id", [](Location location, Engine& engine, Value const& self) -> Value {
        return Value{(Int)self.as_enum().key_id.integer(), location};
    });

    bind_field(enu.id, "name", [](Location location, Engine& engine, Value const& self) -> Value {
        return Value{self.as_enum().key_id.string(), location};
    });

    bind_field(enu.id, "value", [](Location location, Engine& engine, Value const& self) -> Value {
        EnumValue& enum_val = self.as_enum();
        Enum* parent = engine.enums.get(enum_val.parent_id);
        if (!parent)
            ENGINE_PANIC(location, "Unknown enum [", enum_val.parent_id, "]");
        Enum::Entry* entry = parent->get(enum_val.key_id);
        if (!entry)
            ENGINE_PANIC(location, "Enum [", enum_val.parent_id, "] does not have entry [", enum_val.key_id, "]");
        return std::get<Value>(*entry->expr);
    });

    bind_cast(enu.id, id_string, true, [](Location location, Engine& engine, Value const& value) {
        auto& enum_value = value.as_enum();
        return Value{format(enum_value.parent_id, op_link, enum_value.key_id), location};
    });
}

void dawn::Engine::load_struct_standards(Struct const& struc)
{
    auto& members = this->members.get_or_set(struc.id);
    auto& casts = this->casts.get_or_set(struc.id);
    members.clear();
    casts.clear();

    bind_cast(struc.id, id_string, true, [](Location location, Engine& engine, Value const& value) {
        auto& left = value.as_struct();
        StringStream stream;
        stream << left.parent_id;
        if (left.fields.empty())
        {
            stream << op_scope_opn << op_scope_cls;
            return Value{stream.str(), location};
        }
        auto it = left.fields.begin();
        stream << op_scope_opn;
        for (; it != --left.fields.end(); ++it)
            stream << it->first << op_assign << engine.to_string(it->second) << op_split << ' ';
        stream << it->first << op_assign << engine.to_string(it->second) << op_scope_cls;
        return Value{stream.str(), location};
    });
}
