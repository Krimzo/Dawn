#include "engine.h"


static thread_local std::mt19937_64 RAND_ENGINE = []
{
    std::random_device device{};
    std::srand( device() );
    return std::mt19937_64{ device() };
}();

void dawn::Engine::load_standard()
{
    /* CAST */
    bind_func( "to_bool", [this]( Array<ValueRef> const& args ) -> ValueRef
    {
        if ( args.size() != 1 )
            PANIC( "to_bool expected 1 argument, but got ", args.size() );
        return args[0].to_bool( *this );
    } );

    bind_func( "to_int", [this]( Array<ValueRef> const& args ) -> ValueRef
    {
        if ( args.size() != 1 )
            PANIC( "to_int expected 1 argument, but got ", args.size() );
        return args[0].to_int( *this );
    } );

    bind_func( "to_float", [this]( Array<ValueRef> const& args ) -> ValueRef
    {
        if ( args.size() != 1 )
            PANIC( "to_float expected 1 argument, but got ", args.size() );
        return args[0].to_float( *this );
    } );

    bind_func( "to_char", [this]( Array<ValueRef> const& args ) -> ValueRef
    {
        if ( args.size() != 1 )
            PANIC( "to_char expected 1 argument, but got ", args.size() );
        return args[0].to_char( *this );
    } );

    bind_func( "to_string", [this]( Array<ValueRef> const& args ) -> ValueRef
    {
        if ( args.size() != 1 )
            PANIC( "to_string expected 1 argument, but got ", args.size() );
        return args[0].to_string( *this );
    } );

    /* UTIL */
    bind_func( "format", [this]( Array<ValueRef> const& args ) -> ValueRef
    {
        StringStream stream;
        for ( auto& arg : args )
            stream << arg.to_string( *this );

        return stream.str();
    } );

    bind_func( "print", [this]( Array<ValueRef> const& args ) -> ValueRef
    {
        StringStream stream;
        for ( auto& arg : args )
            stream << arg.to_string( *this );
        print( stream.str() );
        return Value{};
    } );

    bind_func( "rand_int", [this]( Array<ValueRef> const& args ) -> ValueRef
    {
        if ( args.size() != 1 )
            PANIC( "rand_int expects 1 argument" );

        return Int( RAND_ENGINE() % args[0].to_int( *this ) );
    } );

    bind_func( "rand_flt", [this]( Array<ValueRef> const& args ) -> ValueRef
    {
        if ( args.size() != 0 )
            PANIC( "rand_flt expects 0 arguments" );

        return (Float) RAND_ENGINE() / UINT64_MAX;
    } );

    /* MATH */
    bind_func( "min", [this]( Array<ValueRef> const& args ) -> ValueRef
    {
        if ( args.size() != 2 )
            PANIC( "min expects 2 arguments" );

        if ( args[0].type() == ValueType::INT )
            return std::min( args[0].to_int( *this ), args[1].to_int( *this ) );

        if ( args[0].type() == ValueType::FLOAT )
            return std::min( args[0].to_float( *this ), args[1].to_float( *this ) );

        PANIC( "min expects an int or float" );
    } );

    bind_func( "max", [this]( Array<ValueRef> const& args ) -> ValueRef
    {
        if ( args.size() != 2 )
            PANIC( "max expects 2 arguments" );

        if ( args[0].type() == ValueType::INT )
            return std::max( args[0].to_int( *this ), args[1].to_int( *this ) );

        if ( args[0].type() == ValueType::FLOAT )
            return std::max( args[0].to_float( *this ), args[1].to_float( *this ) );

        PANIC( "max expects an int or float" );
    } );

    bind_func( "abs", [this]( Array<ValueRef> const& args ) -> ValueRef
    {
        if ( args.size() != 1 )
            PANIC( "abs expects 1 argument" );

        if ( args[0].type() == ValueType::INT )
            return std::abs( args[0].to_int( *this ) );

        if ( args[0].type() == ValueType::FLOAT )
            return std::abs( args[0].to_float( *this ) );

        PANIC( "abs expects an int or float" );
    } );

    bind_func( "sqrt", [this]( Array<ValueRef> const& args ) -> ValueRef
    {
        if ( args.size() != 1 )
            PANIC( "sqrt expects 1 argument" );

        return std::sqrt( args[0].to_float( *this ) );
    } );

    bind_func( "sin", [this]( Array<ValueRef> const& args ) -> ValueRef
    {
        if ( args.size() != 1 )
            PANIC( "sin expects 1 argument" );

        return std::sin( args[0].to_float( *this ) );
    } );

    bind_func( "cos", [this]( Array<ValueRef> const& args ) -> ValueRef
    {
        if ( args.size() != 1 )
            PANIC( "cos expects 1 argument" );

        return std::cos( args[0].to_float( *this ) );
    } );
}
