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
    bind_cfunc( IDSystem::get( "typeid" ), [this]( Value* args, Int arg_count ) -> Value
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

    bind_cfunc( IDSystem::get( "typename" ), [this]( Value* args, Int arg_count ) -> Value
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
    bind_cfunc( IDSystem::get( "bool" ), [this]( Value* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return (Value) args[0].to_bool( *this );
            else if ( arg_count == 0 )
                return Value{ Bool{} };
            else
                PANIC( "bool() expects 1 or 0 arguments, but got ", arg_count );
        } );

    bind_cfunc( IDSystem::get( "int" ), [this]( Value* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return (Value) args[0].to_int( *this );
            else if ( arg_count == 0 )
                return Value{ Int{} };
            else
                PANIC( "int() expects 1 or 0 arguments, but got ", arg_count );
        } );

    bind_cfunc( IDSystem::get( "float" ), [this]( Value* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return (Value) args[0].to_float( *this );
            else if ( arg_count == 0 )
                return Value{ Float{} };
            else
                PANIC( "float() expects 1 or 0 arguments, but got ", arg_count );
        } );

    bind_cfunc( IDSystem::get( "char" ), [this]( Value* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return (Value) args[0].to_char( *this );
            else if ( arg_count == 0 )
                return Value{ Char{} };
            else
                PANIC( "char() expects 1 or 0 arguments, but got ", arg_count );
        } );

    bind_cfunc( IDSystem::get( "string" ), [this]( Value* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return (Value) args[0].to_string( *this );
            else if ( arg_count == 0 )
                return Value{ StringRef{} };
            else
                PANIC( "string() expects 1 or 0 arguments, but got ", arg_count );
        } );

    /* SYSTEM */
    bind_cfunc( IDSystem::get( "exit" ), [this]( Value* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                std::exit( (int) args[0].to_int( *this ) );
            else
                PANIC( "exit() expects 1 argument, but got ", arg_count );
            return {};
        } );

    /* UTILITY */
    bind_cfunc( IDSystem::get( "format" ), [this]( Value* args, Int arg_count ) -> Value
        {
            StringStream stream;
            for ( Int i = 0; i < arg_count; i++ )
                stream << args[i].to_string( *this );
            return (Value) stream.str();
        } );

    bind_cfunc( IDSystem::get( "print" ), [this]( Value* args, Int arg_count ) -> Value
        {
            StringStream stream;
            for ( Int i = 0; i < arg_count; i++ )
                stream << args[i].to_string( *this );
            print( stream.str() );
            return Value{};
        } );

    bind_cfunc( IDSystem::get( "rand_int" ), [this]( Value* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return (Value) Int( RAND_ENGINE() % args[0].to_int( *this ) );
            else
                PANIC( "rand_int() expects 1 argument, but got ", arg_count );
        } );

    bind_cfunc( IDSystem::get( "rand_flt" ), [this]( Value* args, Int arg_count ) -> Value
        {
            if ( arg_count == 0 )
                return (Value) ( (Float) RAND_ENGINE() / UINT64_MAX );
            else
                PANIC( "rand_flt() expects 0 arguments, but got ", arg_count );
        } );

    /* MATH */
    bind_cfunc( IDSystem::get( "min" ), [this]( Value* args, Int arg_count ) -> Value
        {
            if ( arg_count == 2 )
            {
                if ( args[0].type() == ValueType::INT )
                    return (Value) std::min( args[0].to_int( *this ), args[1].to_int( *this ) );
                else if ( args[0].type() == ValueType::FLOAT )
                    return (Value) std::min( args[0].to_float( *this ), args[1].to_float( *this ) );
                else
                    PANIC( "min() expects an int or float" );
            }
            else
                PANIC( "min() expects 2 arguments, but got ", arg_count );
        } );

    bind_cfunc( IDSystem::get( "max" ), [this]( Value* args, Int arg_count ) -> Value
        {
            if ( arg_count == 2 )
            {
                if ( args[0].type() == ValueType::INT )
                    return (Value) std::max( args[0].to_int( *this ), args[1].to_int( *this ) );
                else if ( args[0].type() == ValueType::FLOAT )
                    return (Value) std::max( args[0].to_float( *this ), args[1].to_float( *this ) );
                else
                    PANIC( "max() expects an int or float" );
            }
            else
                PANIC( "max() expects 2 arguments, but got ", arg_count );
        } );

    bind_cfunc( IDSystem::get( "abs" ), [this]( Value* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
            {
                if ( args[0].type() == ValueType::INT )
                    return (Value) std::abs( args[0].to_int( *this ) );
                else if ( args[0].type() == ValueType::FLOAT )
                    return (Value) std::abs( args[0].to_float( *this ) );
                else
                    PANIC( "abs() expects an int or float" );
            }
            else
                PANIC( "abs() expects 1 argument, but got ", arg_count );
        } );

    bind_cfunc( IDSystem::get( "sqrt" ), [this]( Value* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return (Value) std::sqrt( args[0].to_float( *this ) );
            else
                PANIC( "sqrt() expects 1 argument, but got ", arg_count );
        } );

    bind_cfunc( IDSystem::get( "sin" ), [this]( Value* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return (Value) std::sin( args[0].to_float( *this ) );
            else
                PANIC( "sin() expects 1 argument, but got ", arg_count );
        } );

    bind_cfunc( IDSystem::get( "cos" ), [this]( Value* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return (Value) std::cos( args[0].to_float( *this ) );
            else
                PANIC( "cos() expects 1 argument, but got ", arg_count );
        } );
}

void dawn::Engine::load_standard_members()
{
    // string
    bind_member( ValueType::STRING, "count", [this]( Value& self ) -> Value
        {
            return (Value) (Int) self.as_string().size();
        } );

    bind_method( ValueType::STRING, "push", false, 1, [this]( Value& self, Value* args ) -> Value
        {
            self.as_string().push_back( args[0].to_char( *this ) );
            return self;
        } );

    bind_method( ValueType::STRING, "pop", false, 0, [this]( Value& self, Value* args ) -> Value
        {
            self.as_string().pop_back();
            return self;
        } );

    bind_method( ValueType::STRING, "find", true, 1, [this]( Value& self, Value* args ) -> Value
        {
            auto& self_str = self.as_string();
            size_t index = self_str.find( args[0].to_string( *this ) );
            return Value{ Int( index ) };
        } );

    bind_method( ValueType::STRING, "set", false, 2, [this]( Value& self, Value* args ) -> Value
        {
            const Int index = args[0].to_int( *this );
            if ( index < 0 )
                PANIC( "string->set() index must be positive" );
            const String str = args[1].to_string( *this );
            auto& self_str = self.as_string();
            self_str.resize( std::max( index + str.size(), self_str.size() ) );
            std::memcpy( &self_str[index], str.c_str(), str.size() * sizeof( Char ) );
            return self;
        } );

    // enum
    bind_member( ValueType::ENUM, "value", [this]( Value& self ) -> Value
        {
            return self.as_enum().value( *this );
        } );

    // array
    bind_member( ValueType::ARRAY, "count", [this]( Value& self ) -> Value
        {
            return (Value) (Int) self.as_array().data.size();
        } );

    bind_method( ValueType::ARRAY, "push", false, 1, [this]( Value& self, Value* args ) -> Value
        {
            self.as_array().data.emplace_back( args[0] ).unlock_const();
            return self;
        } );

    bind_method( ValueType::ARRAY, "pop", false, 0, [this]( Value& self, Value* args ) -> Value
        {
            self.as_array().data.pop_back();
            return self;
        } );

    bind_method( ValueType::ARRAY, "find", true, 1, [this]( Value& self, Value* args ) -> Value
        {
            auto& self_arr = self.as_array().data;
            auto& to_find = args[0];
            for ( Int i = 0; i < (Int) self_arr.size(); i++ )
            {
                auto& element = self_arr[i];
                if ( element.type() == to_find.type() && element.op_eq( *this, to_find ).as_bool() )
                    return Value{ i };
            }
            return Value{ Int( -1 ) };
        } );

    // range
    bind_member( ValueType::RANGE, "start", [this]( Value& self ) -> Value
        {
            return (Value) self.as_range().start_incl;
        } );

    bind_member( ValueType::RANGE, "end", [this]( Value& self ) -> Value
        {
            return (Value) self.as_range().end_excl;
        } );
}
