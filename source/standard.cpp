#include "engine.h"


static thread_local std::mt19937_64 RAND_ENGINE = []
    {
        std::random_device device{};
        std::srand( device() );
        return std::mt19937_64{ device() };
    }( );

void dawn::Engine::load_standard_functions()
{
    /* TYPE */
    bind_func( IDSystem::get( "typeid" ), [this]( Value* args, Int arg_count ) -> Value
        {
            if ( arg_count != 1 )
                PANIC( "typeid expected 1 argument, but got ", arg_count );

            switch ( args[0].type() )
            {
            case ValueType::NOTHING:
            {
                static const Value id = Value{ IDSystem::get( (String) tp_nothing ) };
                return id;
            }

            case ValueType::BOOL:
            {
                static const Value id = Value{ IDSystem::get( (String) tp_bool ) };
                return id;
            }

            case ValueType::INT:
            {
                static const Value id = Value{ IDSystem::get( (String) tp_int ) };
                return id;
            }

            case ValueType::FLOAT:
            {
                static const Value id = Value{ IDSystem::get( (String) tp_float ) };
                return id;
            }

            case ValueType::CHAR:
            {
                static const Value id = Value{ IDSystem::get( (String) tp_char ) };
                return id;
            }

            case ValueType::STRING:
            {
                static const Value id = Value{ IDSystem::get( (String) tp_string ) };
                return id;
            }

            case ValueType::FUNCTION:
            {
                static const Value id = Value{ IDSystem::get( (String) tp_function ) };
                return id;
            }

            case ValueType::ENUM:
                return Value{ args[0].as_enum().parent->id };

            case ValueType::STRUCT:
                return Value{ args[0].as_struct().parent->id };

            case ValueType::ARRAY:
            {
                static const Value id = Value{ IDSystem::get( (String) tp_array ) };
                return id;
            }

            case ValueType::RANGE:
            {
                static const Value id = Value{ IDSystem::get( (String) tp_range ) };
                return id;
            }

            default:
                PANIC( "unknown value type: ", (Int) args[0].type() );
            }
        } );

    bind_func( IDSystem::get( "typename" ), [this]( Value* args, Int arg_count ) -> Value
        {
            if ( arg_count != 1 )
                PANIC( "typename expected 1 argument, but got ", arg_count );

            switch ( args[0].type() )
            {
            case ValueType::NOTHING:
                return Value{ tp_nothing };

            case ValueType::BOOL:
                return Value{ tp_bool };

            case ValueType::INT:
                return Value{ tp_int };

            case ValueType::FLOAT:
                return Value{ tp_float };

            case ValueType::CHAR:
                return Value{ tp_char };

            case ValueType::STRING:
                return Value{ tp_string };

            case ValueType::FUNCTION:
                return Value{ tp_function };

            case ValueType::ENUM:
                return Value{ IDSystem::get( args[0].as_enum().parent->id ) };

            case ValueType::STRUCT:
                return Value{ IDSystem::get( args[0].as_struct().parent->id ) };

            case ValueType::ARRAY:
                return Value{ tp_array };

            case ValueType::RANGE:
                return Value{ tp_range };

            default:
                PANIC( "unknown value type: ", (Int) args[0].type() );
            }
        } );

    /* CAST */
    bind_func( IDSystem::get( "to_bool" ), [this]( Value* args, Int arg_count ) -> Value
        {
            if ( arg_count != 1 )
                PANIC( "to_bool expected 1 argument, but got ", arg_count );
            return (Value) args[0].to_bool( *this );
        } );

    bind_func( IDSystem::get( "to_int" ), [this]( Value* args, Int arg_count ) -> Value
        {
            if ( arg_count != 1 )
                PANIC( "to_int expected 1 argument, but got ", arg_count );
            return (Value) args[0].to_int( *this );
        } );

    bind_func( IDSystem::get( "to_float" ), [this]( Value* args, Int arg_count ) -> Value
        {
            if ( arg_count != 1 )
                PANIC( "to_float expected 1 argument, but got ", arg_count );
            return (Value) args[0].to_float( *this );
        } );

    bind_func( IDSystem::get( "to_char" ), [this]( Value* args, Int arg_count ) -> Value
        {
            if ( arg_count != 1 )
                PANIC( "to_char expected 1 argument, but got ", arg_count );
            return (Value) args[0].to_char( *this );
        } );

    bind_func( IDSystem::get( "to_string" ), [this]( Value* args, Int arg_count ) -> Value
        {
            if ( arg_count != 1 )
                PANIC( "to_string expected 1 argument, but got ", arg_count );
            return (Value) args[0].to_string( *this );
        } );

    /* UTIL */
    bind_func( IDSystem::get( "format" ), [this]( Value* args, Int arg_count ) -> Value
        {
            StringStream stream;
            for ( Int i = 0; i < arg_count; i++ )
                stream << args[i].to_string( *this );
            return (Value) stream.str();
        } );

    bind_func( IDSystem::get( "print" ), [this]( Value* args, Int arg_count ) -> Value
        {
            StringStream stream;
            for ( Int i = 0; i < arg_count; i++ )
                stream << args[i].to_string( *this );
            print( stream.str() );
            return Value{};
        } );

    bind_func( IDSystem::get( "rand_int" ), [this]( Value* args, Int arg_count ) -> Value
        {
            if ( arg_count != 1 )
                PANIC( "rand_int expects 1 argument" );
            return (Value) Int( RAND_ENGINE() % args[0].to_int( *this ) );
        } );

    bind_func( IDSystem::get( "rand_flt" ), [this]( Value* args, Int arg_count ) -> Value
        {
            if ( arg_count != 0 )
                PANIC( "rand_flt expects 0 arguments" );
            return (Value) ( (Float) RAND_ENGINE() / UINT64_MAX );
        } );

    /* MATH */
    bind_func( IDSystem::get( "min" ), [this]( Value* args, Int arg_count ) -> Value
        {
            if ( arg_count != 2 )
                PANIC( "min expects 2 arguments" );

            if ( args[0].type() == ValueType::INT )
                return (Value) std::min( args[0].to_int( *this ), args[1].to_int( *this ) );

            if ( args[0].type() == ValueType::FLOAT )
                return (Value) std::min( args[0].to_float( *this ), args[1].to_float( *this ) );

            PANIC( "min expects an int or float" );
        } );

    bind_func( IDSystem::get( "max" ), [this]( Value* args, Int arg_count ) -> Value
        {
            if ( arg_count != 2 )
                PANIC( "max expects 2 arguments" );

            if ( args[0].type() == ValueType::INT )
                return (Value) std::max( args[0].to_int( *this ), args[1].to_int( *this ) );

            if ( args[0].type() == ValueType::FLOAT )
                return (Value) std::max( args[0].to_float( *this ), args[1].to_float( *this ) );

            PANIC( "max expects an int or float" );
        } );

    bind_func( IDSystem::get( "abs" ), [this]( Value* args, Int arg_count ) -> Value
        {
            if ( arg_count != 1 )
                PANIC( "abs expects 1 argument" );

            if ( args[0].type() == ValueType::INT )
                return (Value) std::abs( args[0].to_int( *this ) );

            if ( args[0].type() == ValueType::FLOAT )
                return (Value) std::abs( args[0].to_float( *this ) );

            PANIC( "abs expects an int or float" );
        } );

    bind_func( IDSystem::get( "sqrt" ), [this]( Value* args, Int arg_count ) -> Value
        {
            if ( arg_count != 1 )
                PANIC( "sqrt expects 1 argument" );

            return (Value) std::sqrt( args[0].to_float( *this ) );
        } );

    bind_func( IDSystem::get( "sin" ), [this]( Value* args, Int arg_count ) -> Value
        {
            if ( arg_count != 1 )
                PANIC( "sin expects 1 argument" );

            return (Value) std::sin( args[0].to_float( *this ) );
        } );

    bind_func( IDSystem::get( "cos" ), [this]( Value* args, Int arg_count ) -> Value
        {
            if ( arg_count != 1 )
                PANIC( "cos expects 1 argument" );

            return (Value) std::cos( args[0].to_float( *this ) );
        } );
}

void dawn::Engine::load_standard_members()
{
    // string
    add_type_member( ValueType::STRING, "count", [this]( Value& self ) -> Value
        {
            return (Value) (Int) self.as_string().size();
        } );

    add_type_method( ValueType::STRING, "push", false, 1, [this]( Value& self, Value* args ) -> Value
        {
            self.as_string().push_back( args[0].to_char( *this ) );
            return self;
        } );

    add_type_method( ValueType::STRING, "pop", false, 0, [this]( Value& self, Value* args ) -> Value
        {
            self.as_string().pop_back();
            return self;
        } );

    // enum
    add_type_member( ValueType::ENUM, "value", [this]( Value& self ) -> Value
        {
            return self.as_enum().value( *this );
        } );

    // array
    add_type_member( ValueType::ARRAY, "count", [this]( Value& self ) -> Value
        {
            return (Value) (Int) self.as_array().data.size();
        } );

    add_type_method( ValueType::ARRAY, "push", false, 1, [this]( Value& self, Value* args ) -> Value
        {
            self.as_array().data.push_back( args[0] );
            return self;
        } );

    add_type_method( ValueType::ARRAY, "pop", false, 0, [this]( Value& self, Value* args ) -> Value
        {
            self.as_array().data.pop_back();
            return self;
        } );

    // range
    add_type_member( ValueType::RANGE, "start", [this]( Value& self ) -> Value
        {
            return (Value) self.as_range().start_incl;
        } );

    add_type_member( ValueType::RANGE, "end", [this]( Value& self ) -> Value
        {
            return (Value) self.as_range().end_excl;
        } );
}
