#include "engine.h"


static thread_local std::mt19937_64 RAND_ENGINE = []
    {
        std::random_device device{};
        std::srand( device() );
        return std::mt19937_64{ device() };
    }( );

void dawn::Engine::load_standard_operators()
{
    static const ID id_void = IDSystem::get( tp_void );
    static const ID id_bool = IDSystem::get( tp_bool );
    static const ID id_int = IDSystem::get( tp_int );
    static const ID id_float = IDSystem::get( tp_float );
    static const ID id_char = IDSystem::get( tp_char );
    static const ID id_string = IDSystem::get( tp_string );
    static const ID id_range = IDSystem::get( tp_range );
    static const ID id_func = IDSystem::get( tp_function );
    static const ID id_array = IDSystem::get( tp_array );

    // op add
    bind_oper( id_void, OperatorType::ADD, id_int, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ +right.as_int(), location };
        } );

    bind_oper( id_void, OperatorType::ADD, id_float, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ +right.as_float(), location };
        } );

    bind_oper( id_int, OperatorType::ADD, id_int, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_int() + right.as_int(), location };
        } );

    bind_oper( id_int, OperatorType::ADD, id_float, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_int() + right.as_float(), location };
        } );

    bind_oper( id_float, OperatorType::ADD, id_int, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_float() + right.as_int(), location };
        } );

    bind_oper( id_float, OperatorType::ADD, id_float, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_float() + right.as_float(), location };
        } );

    bind_oper( id_string, OperatorType::ADD, id_string, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_string() + right.as_string(), location };
        } );

    bind_oper( id_array, OperatorType::ADD, id_array, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            ArrayValue result;
            result.data.insert( result.data.end(), left.as_array().data.begin(), left.as_array().data.end() );
            result.data.insert( result.data.end(), right.as_array().data.begin(), right.as_array().data.end() );
            return Value{ result, location };
        } );

    // op sub
    bind_oper( id_void, OperatorType::SUB, id_int, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ -right.as_int(), location };
        } );

    bind_oper( id_void, OperatorType::SUB, id_float, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ -right.as_float(), location };
        } );

    bind_oper( id_int, OperatorType::SUB, id_int, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_int() - right.as_int(), location };
        } );

    bind_oper( id_int, OperatorType::SUB, id_float, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_int() - right.as_float(), location };
        } );

    bind_oper( id_float, OperatorType::SUB, id_int, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_float() - right.as_int(), location };
        } );

    bind_oper( id_float, OperatorType::SUB, id_float, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_float() - right.as_float(), location };
        } );

    // op mul
    bind_oper( id_int, OperatorType::MUL, id_int, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_int() * right.as_int(), location };
        } );

    bind_oper( id_int, OperatorType::MUL, id_float, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_int() * right.as_float(), location };
        } );

    bind_oper( id_float, OperatorType::MUL, id_int, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_float() * right.as_int(), location };
        } );

    bind_oper( id_float, OperatorType::MUL, id_float, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_float() * right.as_float(), location };
        } );

    // op div
    bind_oper( id_int, OperatorType::DIV, id_int, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_int() / right.as_int(), location };
        } );

    bind_oper( id_int, OperatorType::DIV, id_float, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_int() / right.as_float(), location };
        } );

    bind_oper( id_float, OperatorType::DIV, id_int, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_float() / right.as_int(), location };
        } );

    bind_oper( id_float, OperatorType::DIV, id_float, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_float() / right.as_float(), location };
        } );

    // op pow
    bind_oper( id_int, OperatorType::POW, id_int, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ (Int) std::pow( left.as_int(), right.as_int() ), location };
        } );

    bind_oper( id_int, OperatorType::POW, id_float, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ std::pow( left.as_int(), right.as_float() ), location };
        } );

    bind_oper( id_float, OperatorType::POW, id_int, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ std::pow( left.as_float(), right.as_int() ), location };
        } );

    bind_oper( id_float, OperatorType::POW, id_float, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ std::pow( left.as_float(), right.as_float() ), location };
        } );

    // op mod
    bind_oper( id_int, OperatorType::MOD, id_int, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_int() % right.as_int(), location };
        } );

    bind_oper( id_int, OperatorType::MOD, id_float, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ dawn_mod( (Float) left.as_int(), right.as_float() ), location };
        } );

    bind_oper( id_float, OperatorType::MOD, id_int, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ dawn_mod( left.as_float(), (Float) right.as_int() ), location };
        } );

    bind_oper( id_float, OperatorType::MOD, id_float, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ dawn_mod( left.as_float(), right.as_float() ), location };
        } );

    // op equals
    bind_oper( id_void, OperatorType::EQ, id_void, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ Bool{ true }, location };
        } );

    bind_oper( id_bool, OperatorType::EQ, id_bool, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_bool() == right.as_bool(), location };
        } );

    bind_oper( id_int, OperatorType::EQ, id_int, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_int() == right.as_int(), location };
        } );

    bind_oper( id_int, OperatorType::EQ, id_float, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_int() == right.as_float(), location };
        } );

    bind_oper( id_float, OperatorType::EQ, id_float, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_float() == right.as_float(), location };
        } );

    bind_oper( id_float, OperatorType::EQ, id_int, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_float() == right.as_int(), location };
        } );

    bind_oper( id_char, OperatorType::EQ, id_char, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_char() == right.as_char(), location };
        } );

    bind_oper( id_string, OperatorType::EQ, id_string, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_string() == right.as_string(), location };
        } );

    // op not equals
    bind_oper( id_void, OperatorType::NOT_EQ, id_void, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ Int{ false }, location };
        } );

    bind_oper( id_bool, OperatorType::NOT_EQ, id_bool, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_bool() != right.as_bool(), location };
        } );

    bind_oper( id_int, OperatorType::NOT_EQ, id_int, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_int() != right.as_int(), location };
        } );

    bind_oper( id_int, OperatorType::NOT_EQ, id_float, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_int() != right.as_float(), location };
        } );

    bind_oper( id_float, OperatorType::NOT_EQ, id_float, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_float() != right.as_float(), location };
        } );

    bind_oper( id_float, OperatorType::NOT_EQ, id_int, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_float() != right.as_int(), location };
        } );

    bind_oper( id_char, OperatorType::NOT_EQ, id_char, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_char() != right.as_char(), location };
        } );

    bind_oper( id_string, OperatorType::NOT_EQ, id_string, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_string() != right.as_string(), location };
        } );

    // op less
    bind_oper( id_int, OperatorType::LESS, id_int, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_int() < right.as_int(), location };
        } );

    bind_oper( id_int, OperatorType::LESS, id_float, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_int() < right.as_float(), location };
        } );

    bind_oper( id_float, OperatorType::LESS, id_float, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_float() < right.as_float(), location };
        } );

    bind_oper( id_float, OperatorType::LESS, id_int, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_float() < right.as_int(), location };
        } );

    // op great
    bind_oper( id_int, OperatorType::GREAT, id_int, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_int() > right.as_int(), location };
        } );

    bind_oper( id_int, OperatorType::GREAT, id_float, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_int() > right.as_float(), location };
        } );

    bind_oper( id_float, OperatorType::GREAT, id_float, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_float() > right.as_float(), location };
        } );

    bind_oper( id_float, OperatorType::GREAT, id_int, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_float() > right.as_int(), location };
        } );

    // op less equals
    bind_oper( id_int, OperatorType::LESS_EQ, id_int, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_int() <= right.as_int(), location };
        } );

    bind_oper( id_int, OperatorType::LESS_EQ, id_float, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_int() <= right.as_float(), location };
        } );

    bind_oper( id_float, OperatorType::LESS_EQ, id_float, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_float() <= right.as_float(), location };
        } );

    bind_oper( id_float, OperatorType::LESS_EQ, id_int, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_float() <= right.as_int(), location };
        } );

    // op great equals
    bind_oper( id_int, OperatorType::GREAT_EQ, id_int, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_int() >= right.as_int(), location };
        } );

    bind_oper( id_int, OperatorType::GREAT_EQ, id_float, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_int() >= right.as_float(), location };
        } );

    bind_oper( id_float, OperatorType::GREAT_EQ, id_float, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_float() >= right.as_float(), location };
        } );

    bind_oper( id_float, OperatorType::GREAT_EQ, id_int, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_float() >= right.as_int(), location };
        } );

    // op not
    bind_oper( id_void, OperatorType::NOT, id_bool, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ !right.as_bool(), location };
        } );

    // op and
    bind_oper( id_bool, OperatorType::AND, id_bool, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_bool() && right.as_bool(), location };
        } );

    // op or
    bind_oper( id_bool, OperatorType::OR, id_bool, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ left.as_bool() || right.as_bool(), location };
        } );

    // op range
    bind_oper( id_int, OperatorType::RANGE, id_int, true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            auto& left = args[0]; auto& right = args[1];
            return Value{ RangeValue{ .start_incl = left.as_int(), .end_excl = right.as_int() }, location };
        } );
}

void dawn::Engine::load_standard_functions()
{
    /* TYPE */
    bind_func( IDSystem::get( "typeid" ), true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return Value{ (Int) args[0].type_id().integer(), location };
            else
                ENGINE_PANIC( location, "typeid expected 1 argument, but got ", arg_count );
        } );

    bind_func( IDSystem::get( "typename" ), true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return Value{ IDSystem::get( args[0].type_id() ), location };
            else
                ENGINE_PANIC( location, "typename expected 1 argument, but got ", arg_count );
        } );

    /* CAST */
    bind_func( IDSystem::get( tp_void ), true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return Value{};
            else if ( arg_count == 0 )
                return Value{};
            else
                ENGINE_PANIC( location, tp_void, "() expects 1 or 0 arguments, but got ", arg_count );
        } );

    bind_func( IDSystem::get( tp_bool ), true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return Value{ args[0].to_bool( engine ), location };
            else if ( arg_count == 0 )
                return Value{ Bool{}, location };
            else
                ENGINE_PANIC( location, tp_bool, "() expects 1 or 0 arguments, but got ", arg_count );
        } );

    bind_func( IDSystem::get( tp_int ), true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return Value{ args[0].to_int( engine ), location };
            else if ( arg_count == 0 )
                return Value{ Int{}, location };
            else
                ENGINE_PANIC( location, tp_int, "() expects 1 or 0 arguments, but got ", arg_count );
        } );

    bind_func( IDSystem::get( tp_float ), true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return Value{ args[0].to_float( engine ), location };
            else if ( arg_count == 0 )
                return Value{ Float{}, location };
            else
                ENGINE_PANIC( location, tp_float, "() expects 1 or 0 arguments, but got ", arg_count );
        } );

    bind_func( IDSystem::get( tp_char ), true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return Value{ args[0].to_char( engine ), location };
            else if ( arg_count == 0 )
                return Value{ Char{}, location };
            else
                ENGINE_PANIC( location, tp_char, "() expects 1 or 0 arguments, but got ", arg_count );
        } );

    bind_func( IDSystem::get( tp_string ), true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return Value{ args[0].to_string( engine ), location };
            else if ( arg_count == 0 )
                return Value{ StringRef{}, location };
            else
                ENGINE_PANIC( location, tp_string, "() expects 1 or 0 arguments, but got ", arg_count );
        } );

    bind_func( IDSystem::get( tp_range ), true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return Value{ args[0].to_range( engine ), location };
            else if ( arg_count == 0 )
                return Value{ RangeValue{}, location };
            else
                ENGINE_PANIC( location, tp_range, "() expects 1 or 0 arguments, but got ", arg_count );
        } );

    bind_func( IDSystem::get( tp_function ), true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return Value{ args[0].to_function( engine ), location };
            else if ( arg_count == 0 )
                return Value{ FunctionValue{}, location };
            else
                ENGINE_PANIC( location, tp_function, "() expects 1 or 0 arguments, but got ", arg_count );
        } );

    bind_func( IDSystem::get( tp_array ), true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return Value{ args[0].to_array( engine ), location };
            else if ( arg_count == 0 )
                return Value{ ArrayValue{}, location };
            else
                ENGINE_PANIC( location, tp_array, "() expects 1 or 0 arguments, but got ", arg_count );
        } );

    /* SYSTEM */
    bind_func( IDSystem::get( "exit" ), false, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                std::exit( (int) args[0].as_int() );
            else
                ENGINE_PANIC( location, "exit() expects 1 argument, but got ", arg_count );
            return {};
        } );

    /* UTILITY */
    bind_func( IDSystem::get( "format" ), true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            StringStream stream;
            for ( Int i = 0; i < arg_count; i++ )
                stream << args[i].to_string( engine );
            return Value{ stream.str(), location };
        } );

    bind_func( IDSystem::get( "print" ), false, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            StringStream stream;
            for ( Int i = 0; i < arg_count; i++ )
                stream << args[i].to_string( engine );
            print( stream.str() );
            return Value{};
        } );

    bind_func( IDSystem::get( "rand_int" ), false, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return Value{ Int( RAND_ENGINE() % args[0].as_int() ), location };
            else
                ENGINE_PANIC( location, "rand_int() expects 1 argument, but got ", arg_count );
        } );

    bind_func( IDSystem::get( "rand_flt" ), false, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            if ( arg_count == 0 )
                return Value{ ( (Float) RAND_ENGINE() / UINT64_MAX ), location };
            else
                ENGINE_PANIC( location, "rand_flt() expects 0 arguments, but got ", arg_count );
        } );

    /* MATH */
    bind_func( IDSystem::get( "min" ), true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            if ( arg_count == 2 )
            {
                if ( args[0].type() == ValueType::INT )
                    return Value{ std::min( args[0].as_int(), args[1].as_int() ), location };
                else if ( args[0].type() == ValueType::FLOAT )
                    return Value{ std::min( args[0].as_float(), args[1].as_float() ), location };
                else
                    ENGINE_PANIC( location, "min() expects an int or float" );
            }
            else
                ENGINE_PANIC( location, "min() expects 2 arguments, but got ", arg_count );
        } );

    bind_func( IDSystem::get( "max" ), true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            if ( arg_count == 2 )
            {
                if ( args[0].type() == ValueType::INT )
                    return Value{ std::max( args[0].as_int(), args[1].as_int() ), location };
                else if ( args[0].type() == ValueType::FLOAT )
                    return Value{ std::max( args[0].as_float(), args[1].as_float() ), location };
                else
                    ENGINE_PANIC( location, "max() expects an int or float" );
            }
            else
                ENGINE_PANIC( location, "max() expects 2 arguments, but got ", arg_count );
        } );

    bind_func( IDSystem::get( "abs" ), true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
            {
                if ( args[0].type() == ValueType::INT )
                    return Value{ std::abs( args[0].as_int() ), location };
                else if ( args[0].type() == ValueType::FLOAT )
                    return Value{ std::abs( args[0].as_float() ), location };
                else
                    ENGINE_PANIC( location, "abs() expects an int or float" );
            }
            else
                ENGINE_PANIC( location, "abs() expects 1 argument, but got ", arg_count );
        } );

    bind_func( IDSystem::get( "sqrt" ), true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return Value{ std::sqrt( args[0].as_float() ), location };
            else
                ENGINE_PANIC( location, "sqrt() expects 1 argument, but got ", arg_count );
        } );

    bind_func( IDSystem::get( "sin" ), true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return Value{ std::sin( args[0].as_float() ), location };
            else
                ENGINE_PANIC( location, "sin() expects 1 argument, but got ", arg_count );
        } );

    bind_func( IDSystem::get( "cos" ), true, []( Location const& location, Engine& engine, Value const* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return Value{ std::cos( args[0].as_float() ), location };
            else
                ENGINE_PANIC( location, "cos() expects 1 argument, but got ", arg_count );
        } );
}

void dawn::Engine::load_standard_members()
{
    // Strings.
    bind_member( ValueType::STRING, "count", []( Location const& location, Engine& engine, Value const& self ) -> Value
        {
            return Value{ (Int) self.as_string().size(), location };
        } );

    bind_method( ValueType::STRING, "push", false, 1, []( Location const& location, Engine& engine, Value const& self, Value const* args ) -> Value
        {
            self.as_string().push_back( args[0].as_char() );
            return self;
        } );

    bind_method( ValueType::STRING, "pop", false, 0, []( Location const& location, Engine& engine, Value const& self, Value const* args ) -> Value
        {
            self.as_string().pop_back();
            return self;
        } );

    bind_method( ValueType::STRING, "find", true, 1, []( Location const& location, Engine& engine, Value const& self, Value const* args ) -> Value
        {
            auto& self_str = self.as_string();
            size_t index = self_str.find( args[0].as_string() );
            return Value{ Int( index ), location };
        } );

    bind_method( ValueType::STRING, "set", false, 2, []( Location const& location, Engine& engine, Value const& self, Value const* args ) -> Value
        {
            const Int index = args[0].as_int();
            if ( index < 0 )
                ENGINE_PANIC( location, "string->set() index must be positive" );
            const String str = args[1].as_string();
            auto& self_str = self.as_string();
            self_str.resize( std::max( index + str.size(), self_str.size() ) );
            std::memcpy( &self_str[index], str.c_str(), str.size() * sizeof( Char ) );
            return self;
        } );

    // Ranges.
    bind_member( ValueType::RANGE, "start", []( Location const& location, Engine& engine, Value const& self ) -> Value
        {
            return Value{ self.as_range().start_incl, location };
        } );

    bind_member( ValueType::RANGE, "end", []( Location const& location, Engine& engine, Value const& self ) -> Value
        {
            return Value{ self.as_range().end_excl, location };
        } );

    // Enums.
    bind_member( ValueType::ENUM, "id", []( Location const& location, Engine& engine, Value const& self ) -> Value
        {
            return Value{ (Int) self.as_enum().key_id.integer(), location };
        } );

    bind_member( ValueType::ENUM, "name", []( Location const& location, Engine& engine, Value const& self ) -> Value
        {
            return Value{ IDSystem::get( self.as_enum().key_id ), location };
        } );

    bind_member( ValueType::ENUM, "value", []( Location const& location, Engine& engine, Value const& self ) -> Value
        {
            return *self.as_enum().value;
        } );

    // Arrays.
    bind_member( ValueType::ARRAY, "count", []( Location const& location, Engine& engine, Value const& self ) -> Value
        {
            return Value{ (Int) self.as_array().data.size(), location };
        } );

    bind_method( ValueType::ARRAY, "push", false, 1, []( Location const& location, Engine& engine, Value const& self, Value const* args ) -> Value
        {
            self.as_array().data.emplace_back( args[0] ).unlock_const();
            return self;
        } );

    bind_method( ValueType::ARRAY, "pop", false, 0, []( Location const& location, Engine& engine, Value const& self, Value const* args ) -> Value
        {
            self.as_array().data.pop_back();
            return self;
        } );

    bind_method( ValueType::ARRAY, "find", true, 1, []( Location const& location, Engine& engine, Value const& self, Value const* args ) -> Value
        {
            auto const& self_arr = self.as_array().data;
            auto const& item = args[0];
            for ( Int i = 0; i < (Int) self_arr.size(); i++ )
            {
                auto& element = self_arr[i];
                if ( element.type() == item.type() && engine.handle_oper( location, element, OperatorType::EQ, item ).as_bool() )
                    return Value{ i, location };
            }
            return Value{ Int( -1 ), location };
        } );
}
