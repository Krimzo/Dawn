#include "engine.h"


static thread_local std::mt19937_64 RAND_ENGINE = []
{
    std::random_device device{};
    std::srand( device() );
    return std::mt19937_64{ device() };
}();

void dawn::Engine::load_standard_functions()
{
    /* TYPE */
    bind_func( IDSystem::get( "typeid" ), [this]( ValueRef const* args, Int arg_count ) -> ValueRef
    {
        if ( arg_count != 1 )
            PANIC( "typeid expected 1 argument, but got ", arg_count );

        switch ( args[0].type() )
        {
        case ValueType::NOTHING:
        {
            static const ValueRef id = ValueRef{ IDSystem::get( (String) tp_nothing ) };
            return id;
        }

        case ValueType::BOOL:
        {
            static const ValueRef id = ValueRef{ IDSystem::get( (String) tp_bool ) };
            return id;
        }

        case ValueType::INT:
        {
            static const ValueRef id = ValueRef{ IDSystem::get( (String) tp_int ) };
            return id;
        }

        case ValueType::FLOAT:
        {
            static const ValueRef id = ValueRef{ IDSystem::get( (String) tp_float ) };
            return id;
        }

        case ValueType::CHAR:
        {
            static const ValueRef id = ValueRef{ IDSystem::get( (String) tp_char ) };
            return id;
        }

        case ValueType::STRING:
        {
            static const ValueRef id = ValueRef{ IDSystem::get( (String) tp_string ) };
            return id;
        }

        case ValueType::FUNCTION:
        {
            static const ValueRef id = ValueRef{ IDSystem::get( (String) tp_function ) };
            return id;
        }

        case ValueType::ENUM:
        {
            return ValueRef{ args[0].as<EnumVal>().parent->id };
        }

        case ValueType::STRUCT:
        {
            return ValueRef{ args[0].as<StructVal>().parent->id };
        }

        case ValueType::ARRAY:
        {
            static const ValueRef id = ValueRef{ IDSystem::get( (String) tp_array ) };
            return id;
        }

        case ValueType::RANGE:
        {
            static const ValueRef id = ValueRef{ IDSystem::get( (String) tp_range ) };
            return id;
        }

        default:
            PANIC( "unknown value type: ", (Int) args[0].type() );
        }
    } );

    bind_func( IDSystem::get( "typename" ), [this]( ValueRef const* args, Int arg_count ) -> ValueRef
    {
        if ( arg_count != 1 )
            PANIC( "typename expected 1 argument, but got ", arg_count );

        switch ( args[0].type() )
        {
        case ValueType::NOTHING:
        {
            return ValueRef{ tp_nothing };
        }

        case ValueType::BOOL:
        {
            return ValueRef{ tp_bool };
        }

        case ValueType::INT:
        {
            return ValueRef{ tp_int };
        }

        case ValueType::FLOAT:
        {
            return ValueRef{ tp_float };
        }

        case ValueType::CHAR:
        {
            return ValueRef{ tp_char };
        }

        case ValueType::STRING:
        {
            return ValueRef{ tp_string };
        }

        case ValueType::FUNCTION:
        {
            return ValueRef{ tp_function };
        }

        case ValueType::ENUM:
        {
            return ValueRef{ IDSystem::get( args[0].as<EnumVal>().parent->id ) };
        }

        case ValueType::STRUCT:
        {
            return ValueRef{ IDSystem::get( args[0].as<StructVal>().parent->id ) };
        }

        case ValueType::ARRAY:
        {
            return ValueRef{ tp_array };
        }

        case ValueType::RANGE:
        {
            return ValueRef{ tp_range };
        }

        default:
            PANIC( "unknown value type: ", (Int) args[0].type() );
        }
    } );

    /* CAST */
    bind_func( IDSystem::get( "to_bool" ), [this]( ValueRef const* args, Int arg_count ) -> ValueRef
    {
        if ( arg_count != 1 )
            PANIC( "to_bool expected 1 argument, but got ", arg_count );
        return (ValueRef) args[0].to_bool( *this );
    } );

    bind_func( IDSystem::get( "to_int" ), [this]( ValueRef const* args, Int arg_count ) -> ValueRef
    {
        if ( arg_count != 1 )
            PANIC( "to_int expected 1 argument, but got ", arg_count );
        return (ValueRef) args[0].to_int( *this );
    } );

    bind_func( IDSystem::get( "to_float" ), [this]( ValueRef const* args, Int arg_count ) -> ValueRef
    {
        if ( arg_count != 1 )
            PANIC( "to_float expected 1 argument, but got ", arg_count );
        return (ValueRef) args[0].to_float( *this );
    } );

    bind_func( IDSystem::get( "to_char" ), [this]( ValueRef const* args, Int arg_count ) -> ValueRef
    {
        if ( arg_count != 1 )
            PANIC( "to_char expected 1 argument, but got ", arg_count );
        return (ValueRef) args[0].to_char( *this );
    } );

    bind_func( IDSystem::get( "to_string" ), [this]( ValueRef const* args, Int arg_count ) -> ValueRef
    {
        if ( arg_count != 1 )
            PANIC( "to_string expected 1 argument, but got ", arg_count );
        return (ValueRef) args[0].to_string( *this );
    } );

    /* UTIL */
    bind_func( IDSystem::get( "format" ), [this]( ValueRef const* args, Int arg_count ) -> ValueRef
    {
        StringStream stream;
        for ( Int i = 0; i < arg_count; i++ )
            stream << args[i].to_string( *this );
        return (ValueRef) stream.str();
    } );

    bind_func( IDSystem::get( "print" ), [this]( ValueRef const* args, Int arg_count ) -> ValueRef
    {
        StringStream stream;
        for ( Int i = 0; i < arg_count; i++ )
            stream << args[i].to_string( *this );
        print( stream.str() );
        return (ValueRef) Value {};
    } );

    bind_func( IDSystem::get( "rand_int" ), [this]( ValueRef const* args, Int arg_count ) -> ValueRef
    {
        if ( arg_count != 1 )
            PANIC( "rand_int expects 1 argument" );
        return (ValueRef) Int( RAND_ENGINE() % args[0].to_int( *this ) );
    } );

    bind_func( IDSystem::get( "rand_flt" ), [this]( ValueRef const* args, Int arg_count ) -> ValueRef
    {
        if ( arg_count != 0 )
            PANIC( "rand_flt expects 0 arguments" );
        return (ValueRef) ((Float) RAND_ENGINE() / UINT64_MAX);
    } );

    /* MATH */
    bind_func( IDSystem::get( "min" ), [this]( ValueRef const* args, Int arg_count ) -> ValueRef
    {
        if ( arg_count != 2 )
            PANIC( "min expects 2 arguments" );

        if ( args[0].type() == ValueType::INT )
            return (ValueRef) std::min( args[0].to_int( *this ), args[1].to_int( *this ) );

        if ( args[0].type() == ValueType::FLOAT )
            return (ValueRef) std::min( args[0].to_float( *this ), args[1].to_float( *this ) );

        PANIC( "min expects an int or float" );
    } );

    bind_func( IDSystem::get( "max" ), [this]( ValueRef const* args, Int arg_count ) -> ValueRef
    {
        if ( arg_count != 2 )
            PANIC( "max expects 2 arguments" );

        if ( args[0].type() == ValueType::INT )
            return (ValueRef) std::max( args[0].to_int( *this ), args[1].to_int( *this ) );

        if ( args[0].type() == ValueType::FLOAT )
            return (ValueRef) std::max( args[0].to_float( *this ), args[1].to_float( *this ) );

        PANIC( "max expects an int or float" );
    } );

    bind_func( IDSystem::get( "abs" ), [this]( ValueRef const* args, Int arg_count ) -> ValueRef
    {
        if ( arg_count != 1 )
            PANIC( "abs expects 1 argument" );

        if ( args[0].type() == ValueType::INT )
            return (ValueRef) std::abs( args[0].to_int( *this ) );

        if ( args[0].type() == ValueType::FLOAT )
            return (ValueRef) std::abs( args[0].to_float( *this ) );

        PANIC( "abs expects an int or float" );
    } );

    bind_func( IDSystem::get( "sqrt" ), [this]( ValueRef const* args, Int arg_count ) -> ValueRef
    {
        if ( arg_count != 1 )
            PANIC( "sqrt expects 1 argument" );

        return (ValueRef) std::sqrt( args[0].to_float( *this ) );
    } );

    bind_func( IDSystem::get( "sin" ), [this]( ValueRef const* args, Int arg_count ) -> ValueRef
    {
        if ( arg_count != 1 )
            PANIC( "sin expects 1 argument" );

        return (ValueRef) std::sin( args[0].to_float( *this ) );
    } );

    bind_func( IDSystem::get( "cos" ), [this]( ValueRef const* args, Int arg_count ) -> ValueRef
    {
        if ( arg_count != 1 )
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

    string_members[IDSystem::get( "count" )] = [this]( ValueRef const& self_val ) -> ValueRef
    {
        auto& self = self_val.as<String>();
        return (ValueRef) (Int) self.size();
    };

    string_members[IDSystem::get( "push" )] = [this]( ValueRef const& self_val ) -> ValueRef
    {
        Function func;
        func.body = [this]( ValueRef const* args, Int arg_count ) -> ValueRef
        {
            if ( arg_count != 2 )
                ENGINE_PANIC( "push expected a value" );

            auto& self = args[0];
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

    enum_members[IDSystem::get( "value" )] = [this]( ValueRef const& self_val ) -> ValueRef
    {
        auto& self = self_val.as<EnumVal>();
        return self.parent->keys_value.at( self.key_id );
    };
}

void dawn::Engine::load_array_members()
{
    auto& array_members = type_members[(Int) ValueType::ARRAY];

    array_members[IDSystem::get( "count" )] = [this]( ValueRef const& self_val ) -> ValueRef
    {
        auto& self = self_val.as<ArrayVal>().data;
        return (ValueRef) (Int) self.size();
    };

    array_members[IDSystem::get( "push" )] = [this]( ValueRef const& self_val ) -> ValueRef
    {
        Function func;
        func.body = [this]( ValueRef const* args, Int arg_count ) -> ValueRef
        {
            if ( arg_count != 2 )
                ENGINE_PANIC( "push expected a value" );

            auto& self = args[0];
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

    range_members[IDSystem::get( "start" )] = [this]( ValueRef const& self_val ) -> ValueRef
    {
        auto& self = self_val.as<RangeVal>();
        return (ValueRef) self.start_incl;
    };

    range_members[IDSystem::get( "end" )] = [this]( ValueRef const& self_val ) -> ValueRef
    {
        auto& self = self_val.as<RangeVal>();
        return (ValueRef) self.end_excl;
    };
}
