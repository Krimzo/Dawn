#include "engine.h"


static thread_local std::mt19937_64 RAND_ENGINE = []
{
    std::random_device device{};
    std::srand( device() );
    return std::mt19937_64{ device() };
}();

void dawn::Engine::load_default_mods()
{
    /* UTIL */
    bind_func( L"format", []( Array<ValueBox> const& args ) -> Value
    {
        StringStream stream;
        for ( auto& arg : args )
            stream << arg.value().to_string();

        return Value{ stream.str() };
    } );

    bind_func( L"print", []( Array<ValueBox> const& args ) -> Value
    {
        StringStream stream;
        for ( auto& arg : args )
            stream << arg.value().to_string();
        print( stream.str() );
        return Value{};
    } );

    bind_func( L"rand_int", []( Array<ValueBox> const& args ) -> Value
    {
        if ( args.size() != 1 )
            PANIC( "rand_int expects 1 argument" );

        return Value{ Int( RAND_ENGINE() % args[0].value().to_int() ) };
    } );

    bind_func( L"rand_flt", []( Array<ValueBox> const& args ) -> Value
    {
        if ( args.size() != 0 )
            PANIC( "rand_flt expects 0 arguments" );

        return Value{ (Float) RAND_ENGINE() / UINT64_MAX };
    } );

    /* MATH */
    bind_func( L"abs", []( Array<ValueBox> const& args ) -> Value
    {
        if ( args.size() != 1 )
            PANIC( "abs expects 1 argument" );

        if ( args[0].value().type() == ValueType::INT )
            return Value{ std::abs( args[0].value().to_int() ) };

        if ( args[0].value().type() == ValueType::FLOAT )
            return Value{ std::abs( args[0].value().to_float() ) };

        PANIC( "abs expects an int or float" );
    } );

    bind_func( L"sqrt", []( Array<ValueBox> const& args ) -> Value
    {
        if ( args.size() != 1 )
            PANIC( "sqrt expects 1 argument" );

        return Value{ std::sqrt( args[0].value().to_float() ) };
    } );

    bind_func( L"sin", []( Array<ValueBox> const& args ) -> Value
    {
        if ( args.size() != 1 )
            PANIC( "sin expects 1 argument" );

        return Value{ std::sin( args[0].value().to_float() ) };
    } );

    bind_func( L"cos", []( Array<ValueBox> const& args ) -> Value
    {
        if ( args.size() != 1 )
            PANIC( "cos expects 1 argument" );

        return Value{ std::cos( args[0].value().to_float() ) };
    } );
}
