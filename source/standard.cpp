#include "engine.h"


static thread_local std::mt19937_64 RAND_ENGINE = []
{
    std::random_device device{};
    std::srand( device() );
    return std::mt19937_64{ device() };
}();

void dawn::Engine::load_standard_functions()
{
    /* CAST */
    bind_func( "to_bool", [this]( Array<ValueRef> const& args ) -> ValueRef
    {
        if ( args.size() != 1 )
            PANIC( "to_bool expected 1 argument, but got ", args.size() );
        return (ValueRef) args[0].to_bool( *this );
    } );

    bind_func( "to_int", [this]( Array<ValueRef> const& args ) -> ValueRef
    {
        if ( args.size() != 1 )
            PANIC( "to_int expected 1 argument, but got ", args.size() );
        return (ValueRef) args[0].to_int( *this );
    } );

    bind_func( "to_float", [this]( Array<ValueRef> const& args ) -> ValueRef
    {
        if ( args.size() != 1 )
            PANIC( "to_float expected 1 argument, but got ", args.size() );
        return (ValueRef) args[0].to_float( *this );
    } );

    bind_func( "to_char", [this]( Array<ValueRef> const& args ) -> ValueRef
    {
        if ( args.size() != 1 )
            PANIC( "to_char expected 1 argument, but got ", args.size() );
        return (ValueRef) args[0].to_char( *this );
    } );

    bind_func( "to_string", [this]( Array<ValueRef> const& args ) -> ValueRef
    {
        if ( args.size() != 1 )
            PANIC( "to_string expected 1 argument, but got ", args.size() );
        return (ValueRef) args[0].to_string( *this );
    } );

    /* UTIL */
    bind_func( "format", [this]( Array<ValueRef> const& args ) -> ValueRef
    {
        StringStream stream;
        for ( auto& arg : args )
            stream << arg.to_string( *this );

        return (ValueRef) stream.str();
    } );

    bind_func( "print", [this]( Array<ValueRef> const& args ) -> ValueRef
    {
        StringStream stream;
        for ( auto& arg : args )
            stream << arg.to_string( *this );
        print( stream.str() );
        return (ValueRef) Value {};
    } );

    bind_func( "rand_int", [this]( Array<ValueRef> const& args ) -> ValueRef
    {
        if ( args.size() != 1 )
            PANIC( "rand_int expects 1 argument" );

        return (ValueRef) Int( RAND_ENGINE() % args[0].to_int( *this ) );
    } );

    bind_func( "rand_flt", [this]( Array<ValueRef> const& args ) -> ValueRef
    {
        if ( args.size() != 0 )
            PANIC( "rand_flt expects 0 arguments" );

        return (ValueRef) ((Float) RAND_ENGINE() / UINT64_MAX);
    } );

    /* MATH */
    bind_func( "min", [this]( Array<ValueRef> const& args ) -> ValueRef
    {
        if ( args.size() != 2 )
            PANIC( "min expects 2 arguments" );

        if ( args[0].type() == ValueType::INT )
            return (ValueRef) std::min( args[0].to_int( *this ), args[1].to_int( *this ) );

        if ( args[0].type() == ValueType::FLOAT )
            return (ValueRef) std::min( args[0].to_float( *this ), args[1].to_float( *this ) );

        PANIC( "min expects an int or float" );
    } );

    bind_func( "max", [this]( Array<ValueRef> const& args ) -> ValueRef
    {
        if ( args.size() != 2 )
            PANIC( "max expects 2 arguments" );

        if ( args[0].type() == ValueType::INT )
            return (ValueRef) std::max( args[0].to_int( *this ), args[1].to_int( *this ) );

        if ( args[0].type() == ValueType::FLOAT )
            return (ValueRef) std::max( args[0].to_float( *this ), args[1].to_float( *this ) );

        PANIC( "max expects an int or float" );
    } );

    bind_func( "abs", [this]( Array<ValueRef> const& args ) -> ValueRef
    {
        if ( args.size() != 1 )
            PANIC( "abs expects 1 argument" );

        if ( args[0].type() == ValueType::INT )
            return (ValueRef) std::abs( args[0].to_int( *this ) );

        if ( args[0].type() == ValueType::FLOAT )
            return (ValueRef) std::abs( args[0].to_float( *this ) );

        PANIC( "abs expects an int or float" );
    } );

    bind_func( "sqrt", [this]( Array<ValueRef> const& args ) -> ValueRef
    {
        if ( args.size() != 1 )
            PANIC( "sqrt expects 1 argument" );

        return (ValueRef) std::sqrt( args[0].to_float( *this ) );
    } );

    bind_func( "sin", [this]( Array<ValueRef> const& args ) -> ValueRef
    {
        if ( args.size() != 1 )
            PANIC( "sin expects 1 argument" );

        return (ValueRef) std::sin( args[0].to_float( *this ) );
    } );

    bind_func( "cos", [this]( Array<ValueRef> const& args ) -> ValueRef
    {
        if ( args.size() != 1 )
            PANIC( "cos expects 1 argument" );

        return (ValueRef) std::cos( args[0].to_float( *this ) );
    } );
}

void dawn::Engine::load_nothing_members()
{
    auto& nothing_members = type_members[(Int) ValueType::NOTHING];

}

void dawn::Engine::load_bool_members()
{
    auto& bool_members = type_members[(Int) ValueType::BOOL];

}

void dawn::Engine::load_int_members()
{
    auto& int_members = type_members[(Int) ValueType::INT];

}

void dawn::Engine::load_float_members()
{
    auto& float_members = type_members[(Int) ValueType::FLOAT];

}

void dawn::Engine::load_char_members()
{
    auto& char_members = type_members[(Int) ValueType::CHAR];

}

void dawn::Engine::load_string_members()
{
    auto& string_members = type_members[(Int) ValueType::STRING];

    string_members[predefines._count.get( id_system )] = [this]( ValueRef const& self_val ) -> ValueRef
    {
        auto& self = self_val.as<String>();
        return (ValueRef) (Int) self.size();
    };

    string_members[predefines._push.get( id_system )] = [this]( ValueRef const& self_val ) -> ValueRef
    {
        Function func;
        func.body = [this]( Array<ValueRef> const& args ) -> ValueRef
        {
            if ( args.size() != 2 )
                ENGINE_PANIC( "push expected a value" );

            auto& self = args.front();
            if ( self.kind() == ValueKind::LET )
                ENGINE_PANIC( "can't push to a let string" );

            auto& self_val = self.as<String>();
            self_val.push_back( args[1].to_char( *this ) );
            return self;
        };
        return (ValueRef) func;
    };
}

void dawn::Engine::load_function_members()
{
    auto& function_members = type_members[(Int) ValueType::FUNCTION];

}

void dawn::Engine::load_enum_members()
{
    auto& enum_members = type_members[(Int) ValueType::ENUM];

    enum_members[predefines._value.get( id_system )] = [this]( ValueRef const& self_val ) -> ValueRef
    {
        auto& self = self_val.as<EnumVal>();
        return self.parent->keys_value.at( self.key.get( id_system ) );
    };
}

void dawn::Engine::load_array_members()
{
    auto& array_members = type_members[(Int) ValueType::ARRAY];

    array_members[predefines._count.get( id_system )] = [this]( ValueRef const& self_val ) -> ValueRef
    {
        auto& self = self_val.as<ArrayVal>().data;
        return (ValueRef) (Int) self.size();
    };

    array_members[predefines._push.get( id_system )] = [this]( ValueRef const& self_val ) -> ValueRef
    {
        Function func;
        func.body = [this]( Array<ValueRef> const& args ) -> ValueRef
        {
            if ( args.size() != 2 )
                ENGINE_PANIC( "push expected a value" );

            auto& self = args.front();
            if ( self.kind() == ValueKind::LET )
                ENGINE_PANIC( "can't push to a let array" );

            auto& self_val = self.as<ArrayVal>().data;
            self_val.emplace_back( args[1].value(), self.kind() );
            return self;
        };
        return (ValueRef) func;
    };
}

void dawn::Engine::load_range_members()
{
    auto& range_members = type_members[(Int) ValueType::RANGE];

    range_members[predefines._start.get( id_system )] = [this]( ValueRef const& self_val ) -> ValueRef
    {
        auto& self = self_val.as<RangeVal>();
        return (ValueRef) self.start_incl;
    };

    range_members[predefines._end.get( id_system )] = [this]( ValueRef const& self_val ) -> ValueRef
    {
        auto& self = self_val.as<RangeVal>();
        return (ValueRef) self.end_excl;
    };
}
