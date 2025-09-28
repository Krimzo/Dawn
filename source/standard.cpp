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
    bind_cfunc( IDSystem::get( "typeid" ), [this]( Location const& location, Value* args, Int arg_count ) -> Value
        {
            if ( arg_count != 1 )
                ENGINE_PANIC( location, "typeid expected 1 argument, but got ", arg_count );

            switch ( args[0].type() )
            {
            case ValueType::NOTHING:
            {
                static const Value id = Value{ IDSystem::get( tp_nothing ) };
                return id;
            }

            case ValueType::BOOL:
            {
                static const Value id = Value{ IDSystem::get( tp_bool ) };
                return id;
            }

            case ValueType::INT:
            {
                static const Value id = Value{ IDSystem::get( tp_int ) };
                return id;
            }

            case ValueType::FLOAT:
            {
                static const Value id = Value{ IDSystem::get( tp_float ) };
                return id;
            }

            case ValueType::CHAR:
            {
                static const Value id = Value{ IDSystem::get( tp_char ) };
                return id;
            }

            case ValueType::STRING:
            {
                static const Value id = Value{ IDSystem::get( tp_string ) };
                return id;
            }

            case ValueType::FUNCTION:
            {
                static const Value id = Value{ IDSystem::get( tp_function ) };
                return id;
            }

            case ValueType::ENUM:
                return Value{ args[0].as_enum().parent_id };

            case ValueType::STRUCT:
                return Value{ args[0].as_struct().parent->id };

            case ValueType::ARRAY:
            {
                static const Value id = Value{ IDSystem::get( tp_array ) };
                return id;
            }

            case ValueType::RANGE:
            {
                static const Value id = Value{ IDSystem::get( tp_range ) };
                return id;
            }

            default:
                ENGINE_PANIC( location, "unknown value type: ", (Int) args[0].type() );
            }
        } );

    bind_cfunc( IDSystem::get( "typename" ), [this]( Location const& location, Value* args, Int arg_count ) -> Value
        {
            if ( arg_count != 1 )
                ENGINE_PANIC( location, "typename expected 1 argument, but got ", arg_count );

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
                return Value{ IDSystem::get( args[0].as_enum().parent_id ) };

            case ValueType::STRUCT:
                return Value{ IDSystem::get( args[0].as_struct().parent->id ) };

            case ValueType::ARRAY:
                return Value{ tp_array };

            case ValueType::RANGE:
                return Value{ tp_range };

            default:
                ENGINE_PANIC( location, "unknown value type: ", (Int) args[0].type() );
            }
        } );

    /* CAST */
    bind_cfunc( IDSystem::get( "bool" ), [this]( Location const& location, Value* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return (Value) args[0].to_bool( *this, location );
            else if ( arg_count == 0 )
                return Value{ Bool{} };
            else
                ENGINE_PANIC( location, "bool() expects 1 or 0 arguments, but got ", arg_count );
        } );

    bind_cfunc( IDSystem::get( "int" ), [this]( Location const& location, Value* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return (Value) args[0].to_int( *this, location );
            else if ( arg_count == 0 )
                return Value{ Int{} };
            else
                ENGINE_PANIC( location, "int() expects 1 or 0 arguments, but got ", arg_count );
        } );

    bind_cfunc( IDSystem::get( "float" ), [this]( Location const& location, Value* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return (Value) args[0].to_float( *this, location );
            else if ( arg_count == 0 )
                return Value{ Float{} };
            else
                ENGINE_PANIC( location, "float() expects 1 or 0 arguments, but got ", arg_count );
        } );

    bind_cfunc( IDSystem::get( "char" ), [this]( Location const& location, Value* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return (Value) args[0].to_char( *this, location );
            else if ( arg_count == 0 )
                return Value{ Char{} };
            else
                ENGINE_PANIC( location, "char() expects 1 or 0 arguments, but got ", arg_count );
        } );

    bind_cfunc( IDSystem::get( "string" ), [this]( Location const& location, Value* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return (Value) args[0].to_string( *this, location );
            else if ( arg_count == 0 )
                return Value{ StringRef{} };
            else
                ENGINE_PANIC( location, "string() expects 1 or 0 arguments, but got ", arg_count );
        } );

    /* SYSTEM */
    bind_cfunc( IDSystem::get( "exit" ), [this]( Location const& location, Value* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                std::exit( (int) args[0].to_int( *this, location ) );
            else
                ENGINE_PANIC( location, "exit() expects 1 argument, but got ", arg_count );
            return {};
        } );

    /* UTILITY */
    bind_cfunc( IDSystem::get( "format" ), [this]( Location const& location, Value* args, Int arg_count ) -> Value
        {
            StringStream stream;
            for ( Int i = 0; i < arg_count; i++ )
                stream << args[i].to_string( *this, location );
            return (Value) stream.str();
        } );

    bind_cfunc( IDSystem::get( "print" ), [this]( Location const& location, Value* args, Int arg_count ) -> Value
        {
            StringStream stream;
            for ( Int i = 0; i < arg_count; i++ )
                stream << args[i].to_string( *this, location );
            print( stream.str() );
            return Value{};
        } );

    bind_cfunc( IDSystem::get( "rand_int" ), [this]( Location const& location, Value* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return (Value) Int( RAND_ENGINE() % args[0].to_int( *this, location ) );
            else
                ENGINE_PANIC( location, "rand_int() expects 1 argument, but got ", arg_count );
        } );

    bind_cfunc( IDSystem::get( "rand_flt" ), [this]( Location const& location, Value* args, Int arg_count ) -> Value
        {
            if ( arg_count == 0 )
                return (Value) ( (Float) RAND_ENGINE() / UINT64_MAX );
            else
                ENGINE_PANIC( location, "rand_flt() expects 0 arguments, but got ", arg_count );
        } );

    /* MATH */
    bind_cfunc( IDSystem::get( "min" ), [this]( Location const& location, Value* args, Int arg_count ) -> Value
        {
            if ( arg_count == 2 )
            {
                if ( args[0].type() == ValueType::INT )
                    return (Value) std::min( args[0].to_int( *this, location ), args[1].to_int( *this, location ) );
                else if ( args[0].type() == ValueType::FLOAT )
                    return (Value) std::min( args[0].to_float( *this, location ), args[1].to_float( *this, location ) );
                else
                    ENGINE_PANIC( location, "min() expects an int or float" );
            }
            else
                ENGINE_PANIC( location, "min() expects 2 arguments, but got ", arg_count );
        } );

    bind_cfunc( IDSystem::get( "max" ), [this]( Location const& location, Value* args, Int arg_count ) -> Value
        {
            if ( arg_count == 2 )
            {
                if ( args[0].type() == ValueType::INT )
                    return (Value) std::max( args[0].to_int( *this, location ), args[1].to_int( *this, location ) );
                else if ( args[0].type() == ValueType::FLOAT )
                    return (Value) std::max( args[0].to_float( *this, location ), args[1].to_float( *this, location ) );
                else
                    ENGINE_PANIC( location, "max() expects an int or float" );
            }
            else
                ENGINE_PANIC( location, "max() expects 2 arguments, but got ", arg_count );
        } );

    bind_cfunc( IDSystem::get( "abs" ), [this]( Location const& location, Value* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
            {
                if ( args[0].type() == ValueType::INT )
                    return (Value) std::abs( args[0].to_int( *this, location ) );
                else if ( args[0].type() == ValueType::FLOAT )
                    return (Value) std::abs( args[0].to_float( *this, location ) );
                else
                    ENGINE_PANIC( location, "abs() expects an int or float" );
            }
            else
                ENGINE_PANIC( location, "abs() expects 1 argument, but got ", arg_count );
        } );

    bind_cfunc( IDSystem::get( "sqrt" ), [this]( Location const& location, Value* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return (Value) std::sqrt( args[0].to_float( *this, location ) );
            else
                ENGINE_PANIC( location, "sqrt() expects 1 argument, but got ", arg_count );
        } );

    bind_cfunc( IDSystem::get( "sin" ), [this]( Location const& location, Value* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return (Value) std::sin( args[0].to_float( *this, location ) );
            else
                ENGINE_PANIC( location, "sin() expects 1 argument, but got ", arg_count );
        } );

    bind_cfunc( IDSystem::get( "cos" ), [this]( Location const& location, Value* args, Int arg_count ) -> Value
        {
            if ( arg_count == 1 )
                return (Value) std::cos( args[0].to_float( *this, location ) );
            else
                ENGINE_PANIC( location, "cos() expects 1 argument, but got ", arg_count );
        } );
}

void dawn::Engine::load_standard_members()
{
    // Strings.
    bind_member( ValueType::STRING, "count", [this]( Location const& location, Value& self ) -> Value
        {
            return (Value) (Int) self.as_string().size();
        } );

    bind_method( ValueType::STRING, "push", false, 1, [this]( Location const& location, Value& self, Value* args ) -> Value
        {
            self.as_string().push_back( args[0].to_char( *this, location ) );
            return self;
        } );

    bind_method( ValueType::STRING, "pop", false, 0, [this]( Location const& location, Value& self, Value* args ) -> Value
        {
            self.as_string().pop_back();
            return self;
        } );

    bind_method( ValueType::STRING, "find", true, 1, [this]( Location const& location, Value& self, Value* args ) -> Value
        {
            auto& self_str = self.as_string();
            size_t index = self_str.find( args[0].to_string( *this, location ) );
            return Value{ Int( index ) };
        } );

    bind_method( ValueType::STRING, "set", false, 2, [this]( Location const& location, Value& self, Value* args ) -> Value
        {
            const Int index = args[0].to_int( *this, location );
            if ( index < 0 )
                ENGINE_PANIC( location, "string->set() index must be positive" );
            const String str = args[1].to_string( *this, location );
            auto& self_str = self.as_string();
            self_str.resize( std::max( index + str.size(), self_str.size() ) );
            std::memcpy( &self_str[index], str.c_str(), str.size() * sizeof( Char ) );
            return self;
        } );

    // Enums.
    bind_member( ValueType::ENUM, "value", [this]( Location const& location, Value& self ) -> Value
        {
            return *self.as_enum().value;
        } );

    // Arrays.
    bind_member( ValueType::ARRAY, "count", [this]( Location const& location, Value& self ) -> Value
        {
            return (Value) (Int) self.as_array().data.size();
        } );

    bind_method( ValueType::ARRAY, "push", false, 1, [this]( Location const& location, Value& self, Value* args ) -> Value
        {
            self.as_array().data.emplace_back( args[0] ).unlock_const();
            return self;
        } );

    bind_method( ValueType::ARRAY, "pop", false, 0, [this]( Location const& location, Value& self, Value* args ) -> Value
        {
            self.as_array().data.pop_back();
            return self;
        } );

    bind_method( ValueType::ARRAY, "find", true, 1, [this]( Location const& location, Value& self, Value* args ) -> Value
        {
            auto& self_arr = self.as_array().data;
            auto& to_find = args[0];
            for ( Int i = 0; i < (Int) self_arr.size(); i++ )
            {
                auto& element = self_arr[i];
                if ( element.type() == to_find.type() && element.op_eq( *this, location, to_find ).as_bool() )
                    return Value{ i };
            }
            return Value{ Int( -1 ) };
        } );

    // Ranges.
    bind_member( ValueType::RANGE, "start", [this]( Location const& location, Value& self ) -> Value
        {
            return (Value) self.as_range().start_incl;
        } );

    bind_member( ValueType::RANGE, "end", [this]( Location const& location, Value& self ) -> Value
        {
            return (Value) self.as_range().end_excl;
        } );
}
