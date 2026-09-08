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

    /* DAWN */
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

    /* TYPE */
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

    /* SYSTEM */
    bind_function("exit", false, [](Location location, Engine& engine, Value const* args, Int arg_count) -> Value {
        if (arg_count == 1)
            std::exit((int)args[0].as_int());
        else
            ENGINE_PANIC(location, "exit() expects 1 argument, but got ", arg_count);
        return {};
    });

    /* UTILITY */
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

    /* MATH */
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
    // Strings.
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

    // Ranges.
    bind_field(id_range, "start", [](Location location, Engine& engine, Value const& self) -> Value {
        return Value{&self.as_range().start, self.is_const(), location};
    });

    bind_field(id_range, "end", [](Location location, Engine& engine, Value const& self) -> Value {
        return Value{&self.as_range().end, self.is_const(), location};
    });

    bind_field(id_range, "inclusive", [](Location location, Engine& engine, Value const& self) -> Value {
        return Value{&self.as_range().inclusive, self.is_const(), location};
    });

    // Enums.
    // bind_field(ValueType::ENUM, "id", [](Location location, Engine& engine, Value const& self) -> Value {
    //     return Value{(Int)self.as_enum().key_id.integer(), location};
    // });

    // bind_field(ValueType::ENUM, "name", [](Location location, Engine& engine, Value const& self) -> Value {
    //     return Value{self.as_enum().key_id.string(), location};
    // });

    // bind_field(ValueType::ENUM, "value", [](Location location, Engine& engine, Value const& self) -> Value {
    //     EnumValue& enum_val = self.as_enum();
    //     Enum* parent = engine.enums.get(enum_val.parent_id);
    //     if (!parent)
    //         ENGINE_PANIC(location, "Unknown enum [", enum_val.parent_id, "]");
    //     Enum::Entry* entry = parent->get(enum_val.key_id);
    //     if (!entry)
    //         ENGINE_PANIC(location, "Enum [", enum_val.parent_id, "] does not have entry [", enum_val.key_id, "]");
    //     return std::get<Value>(*entry->expr);
    // });

    // Arrays.
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
    /* TO BOOL */
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
    // dawn::Int dawn::Engine::to_int(Value const& value)
    // {
    //     switch (value.type())
    //     {
    //     case ValueType::VOID:
    //         return Int{};

    //     case ValueType::BOOL:
    //         return (Int)as_bool();

    //     case ValueType::INT:
    //         return as_int();

    //     case ValueType::FLOAT:
    //         return (Int)as_float();

    //     case ValueType::CHAR:
    //         return (Int)as_char();

    //     case ValueType::STRING: {
    //         if (auto optres = parse_int(as_string()))
    //             return *optres;
    //         throw Value{dawn::format("string \"", as_string(), "\" to int failed"), location()};
    //     }

    //     case ValueType::STRUCT: {
    //         auto& left = as_struct();
    //         auto* method = left.get_method(id_int);
    //         if (!method)
    //             ENGINE_PANIC(location(), "can not convert struct [", left.parent_id, "] to int");
    //         return engine.handle_func(location(), *method, this, 1).as_int();
    //     }

    //     default:
    //         ENGINE_PANIC(location(), "can not convert [", type(), "] to int");
    //     }
    // }

    // to float
    // dawn::Float dawn::Engine::to_float(Value const& value)
    // {
    //     switch (type())
    //     {
    //     case ValueType::VOID:
    //         return Float{};

    //     case ValueType::BOOL:
    //         return (Float)as_bool();

    //     case ValueType::INT:
    //         return (Float)as_int();

    //     case ValueType::FLOAT:
    //         return as_float();

    //     case ValueType::CHAR:
    //         return (Float)as_char();

    //     case ValueType::STRING: {
    //         if (auto optres = parse_float(as_string()))
    //             return *optres;
    //         throw Value{dawn::format("string \"", as_string(), "\" to float failed"), location()};
    //     }

    //     case ValueType::STRUCT: {
    //         auto& left = as_struct();
    //         auto* method = left.get_method(id_float);
    //         if (!method)
    //             ENGINE_PANIC(location(), "can not convert struct [", left.parent_id, "] to float");
    //         return engine.handle_func(location(), *method, this, 1).as_float();
    //     }

    //     default:
    //         ENGINE_PANIC(location(), "can not convert [", type(), "] to float");
    //     }
    // }

    /* TO CHAR */
    // dawn::Char dawn::Engine::to_char(Value const& value)
    // {
    //     switch (type())
    //     {
    //     case ValueType::VOID:
    //         return Char{};

    //     case ValueType::BOOL:
    //         return as_bool() ? kw_true.front() : kw_false.front();

    //     case ValueType::INT:
    //         return (Char)as_int();

    //     case ValueType::FLOAT:
    //         return (Char)as_float();

    //     case ValueType::CHAR:
    //         return as_char();

    //     case ValueType::STRING: {
    //         auto& str = as_string();
    //         if (str.empty())
    //             return Char{};
    //         return str.front();
    //     }

    //     case ValueType::STRUCT: {
    //         auto& left = as_struct();
    //         auto* method = left.get_method(id_char);
    //         if (!method)
    //             ENGINE_PANIC(location(), "can not convert struct [", left.parent_id, "] to char");
    //         return engine.handle_func(location(), *method, this, 1).as_char();
    //     }

    //     default:
    //         ENGINE_PANIC(location(), "can not convert [", type(), "] to char");
    //     }
    // }

    // to string
    // dawn::String dawn::Engine::to_string(Value const& value)
    // {
    //     switch (type())
    //     {
    //     case ValueType::VOID:
    //         return String{};

    //     case ValueType::BOOL:
    //         return String{as_bool() ? kw_true : kw_false};

    //     case ValueType::INT:
    //         return std::to_string(as_int());

    //     case ValueType::FLOAT:
    //         return format(std::showpoint, as_float());

    //     case ValueType::CHAR:
    //         return String{as_char()};

    //     case ValueType::STRING:
    //         return as_string();

    //     case ValueType::RANGE: {
    //         auto& value = as_range();
    //         if (value.inclusive)
    //             return format(op_array_opn, value.start, " -> ", value.end, op_array_cls);
    //         else
    //             return format(op_array_opn, value.start, " -> ", value.end, op_expr_cls);
    //     }

    //     case ValueType::FUNCTION: {
    //         StringStream stream;
    //         auto& func = as_function();

    //         if (func.is_global())
    //         {
    //             auto& global = func.as_global();
    //             stream << global.id << op_expr_opn;
    //         }
    //         else if (func.is_method())
    //         {
    //             auto& method = func.as_method();
    //             stream << method.self->as_struct().parent_id << op_access << method.id << op_expr_opn;
    //         }
    //         else
    //         {
    //             stream << "lambda" << op_lambda;
    //         }

    //         if (auto* dfunc = func.dfunction())
    //         {
    //             if (!dfunc->args.empty())
    //             {
    //                 for (Int i = 0; i < (Int)dfunc->args.size() - 1; i++)
    //                     stream << dfunc->args[i].type << ' ' << dfunc->args[i].id << op_split << ' ';
    //                 stream << dfunc->args.back().type << ' ' << dfunc->args.back().id;
    //             }
    //         }

    //         stream << (func.is_lambda() ? op_lambda : op_expr_cls);
    //         return stream.str();
    //     }

    //     case ValueType::ARRAY: {
    //         auto& value = as_array();
    //         if (value.data.empty())
    //             return format(op_array_opn, op_array_cls);

    //         StringStream stream;
    //         stream << op_array_opn;
    //         for (Int i = 0; i < (Int)value.data.size() - 1; i++)
    //             stream << value.data[i].to_string(engine) << op_split << ' ';
    //         stream << value.data.back().to_string(engine) << op_array_cls;
    //         return stream.str();
    //     }

    //     case ValueType::ENUM: {
    //         auto const& value = as_enum();
    //         return format(value.parent_id, op_link, value.key_id);
    //     }

    //     case ValueType::STRUCT: {
    //         auto& left = as_struct();
    //         if (auto* method = left.get_method(id_string))
    //         {
    //             return engine.handle_func(location(), *method, this, 1).as_string();
    //         }
    //         else
    //         {
    //             StringStream stream;
    //             stream << left.parent_id;
    //             if (left.members.empty())
    //             {
    //                 stream << op_scope_opn << op_scope_cls;
    //                 return stream.str();
    //             }

    //             auto it = left.members.begin();
    //             stream << op_scope_opn;
    //             for (; it != --left.members.end(); ++it)
    //                 stream << it->id << op_assign << it->value.to_string(engine) << op_split << ' ';
    //             stream << it->id << op_assign << it->value.to_string(engine) << op_scope_cls;
    //             return stream.str();
    //         }
    //     }

    //     default:
    //         ENGINE_PANIC(location(), "can not convert [", type(), "] to string");
    //     }
    // }

    // to string
    // dawn::RangeValue dawn::Engine::to_range(Value const& value)
    // {
    //     switch (type())
    //     {
    //     case ValueType::VOID:
    //         return RangeValue{};

    //     case ValueType::INT:
    //         return RangeValue{0, as_int()};

    //     case ValueType::RANGE:
    //         return as_range();

    //     case ValueType::STRUCT: {
    //         auto& left = as_struct();
    //         auto* method = left.get_method(id_range);
    //         if (!method)
    //             ENGINE_PANIC(location(), "can not convert struct [", left.parent_id, "] to range");
    //         return engine.handle_func(location(), *method, this, 1).as_range();
    //     }

    //     default:
    //         ENGINE_PANIC(location(), "can not convert [", type(), "] to range");
    //     }
    // }

    // to function
    // dawn::FunctionValue dawn::Engine::to_function(Value const& value)
    // {
    //     switch (type())
    //     {
    //     case ValueType::FUNCTION:
    //         return as_function();

    //     case ValueType::STRUCT: {
    //         auto& left = as_struct();
    //         auto* method = left.get_method(id_function);
    //         if (!method)
    //             ENGINE_PANIC(location(), "can not convert struct [", left.parent_id, "] to function");
    //         return engine.handle_func(location(), *method, this, 1).as_function();
    //     }

    //     default:
    //         ENGINE_PANIC(location(), "can not convert [", type(), "] to function");
    //     }
    // }

    // to array
    // dawn::ArrayValue dawn::Value::to_array(Engine& engine) const
    // {
    //     switch (type())
    //     {
    //     case ValueType::VOID:
    //         return ArrayValue{};

    //     case ValueType::STRING: {
    //         auto& strval = as_string();
    //         ArrayValue result;
    //         result.data.reserve(strval.size());
    //         for (Char c : strval)
    //             result.data.emplace_back(c, location());
    //         return result;
    //     }

    //     case ValueType::ARRAY:
    //         return as_array();

    //     case ValueType::STRUCT: {
    //         auto& left = as_struct();
    //         auto* method = left.get_method(id_array);
    //         if (!method)
    //             ENGINE_PANIC(location(), "can not convert struct [", left.parent_id, "] to array");
    //         return engine.handle_func(location(), *method, this, 1).as_array();
    //     }

    //     default:
    //         ENGINE_PANIC(location(), "can not convert [", type(), "] to array");
    //     }
    // }
}
