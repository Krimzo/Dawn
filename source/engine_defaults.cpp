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
    bind_func( L"format", []( Array<Ref<Value>> const& args ) -> Ref<Value>
    {
        StringStream stream;
        for ( auto& arg : args )
            stream << arg->to_string();
        auto result = StringValue::make();
        result->value = stream.str();
        return result;
    } );

    bind_func( L"print", []( Array<Ref<Value>> const& args ) -> Ref<Value>
    {
        StringStream stream;
        for ( auto& arg : args )
            stream << arg->to_string();
        print( stream.str() );
        return nullptr;
    } );

    bind_func( L"rand_int", []( Array<Ref<Value>> const& args ) -> Ref<Value>
    {
        if ( args.size() != 1 )
            PANIC( "rand_int expects 1 argument" );

        auto result = IntValue::make();
        result->value = RAND_ENGINE() % args[0]->to_int();
        return result;
    } );

    bind_func( L"rand_flt", []( Array<Ref<Value>> const& args ) -> Ref<Value>
    {
        if ( args.size() != 0 )
            PANIC( "rand_flt expects 0 arguments" );

        auto result = FloatValue::make();
        result->value = (Float) RAND_ENGINE() / UINT64_MAX;
        return result;
    } );

    /* MATH */
    bind_func( L"abs", []( Array<Ref<Value>> const& args ) -> Ref<Value>
    {
        if ( args.size() != 1 )
            PANIC( "abs expects 1 argument" );

        if ( args[0]->type() == tp_int )
            return make_int_value( std::abs( args[0]->to_int() ) );

        if ( args[0]->type() == tp_float )
            return make_float_value( std::abs( args[0]->to_float() ) );

        PANIC( "abs expects an int or float" );
    } );

    bind_func( L"sqrt", []( Array<Ref<Value>> const& args ) -> Ref<Value>
    {
        if ( args.size() != 1 )
            PANIC( "sqrt expects 1 argument" );

        auto result = FloatValue::make();
        result->value = std::sqrt( args[0]->to_float() );
        return result;
    } );

    bind_func( L"sin", []( Array<Ref<Value>> const& args ) -> Ref<Value>
    {
        if ( args.size() != 1 )
            PANIC( "sin expects 1 argument" );

        auto result = FloatValue::make();
        result->value = std::sin( args[0]->to_float() );
        return result;
    } );

    bind_func( L"cos", []( Array<Ref<Value>> const& args ) -> Ref<Value>
    {
        if ( args.size() != 1 )
            PANIC( "cos expects 1 argument" );

        auto result = FloatValue::make();
        result->value = std::cos( args[0]->to_float() );
        return result;
    } );
}
