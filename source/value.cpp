#include "value.h"
#include "pool.h"
#include "engine.h"


#define OP_HELPER(op) static const Int __##op = IDSystem::get( (String) op_##op )
#define ID_HELPER(id) static const Int _##id = IDSystem::get( #id )

namespace dawn
{
OP_HELPER( add );
OP_HELPER( sub );
OP_HELPER( mul );
OP_HELPER( div );
OP_HELPER( pow );
OP_HELPER( mod );
OP_HELPER( cmpr );

ID_HELPER( to_bool );
ID_HELPER( to_int );
ID_HELPER( to_float );
ID_HELPER( to_char );
ID_HELPER( to_string );
}

dawn::Value dawn::EnumValue::value( Engine& engine ) const
{
    auto& expr = parent->get( key_id )->expr;
    return engine.handle_expr( expr.value() ).clone();
}

dawn::StructValue::StructValue( StructValue const& other )
{
    parent = other.parent;
    for ( auto& [key, val] : other.members )
        members[key] = val.clone();
}

dawn::StructValue& dawn::StructValue::operator=( StructValue const& other )
{
    if ( this != &other )
    {
        parent = other.parent;
        members.clear();
        for ( auto& [key, val] : other.members )
            members[key] = val.clone();
    }
    return *this;
}

dawn::Value* dawn::StructValue::get_member( Int id )
{
    auto it = members.find( id );
    if ( it == members.end() )
        return nullptr;
    return &it->second;
}

dawn::Function* dawn::StructValue::get_method( Int id, Bool has_no_args )
{
    auto it = members.find( id );
    if ( it == members.end() )
        return nullptr;

    if ( it->second.type() != ValueType::FUNCTION )
        return nullptr;

    auto& func = it->second.as_function();
    if ( func.type() != FunctionType::METHOD )
        return nullptr;

    if ( has_no_args && func.args.size() != 1 )
        return nullptr;
    return &func;
}

dawn::ArrayValue::ArrayValue( ArrayValue const& other )
{
    data.reserve( other.data.size() );
    for ( auto& value : other.data )
        data.push_back( value.clone() );
}

dawn::ArrayValue& dawn::ArrayValue::operator=( ArrayValue const& other )
{
    if ( this != &other )
    {
        data.clear();
        data.reserve( other.data.size() );
        for ( auto& value : other.data )
            data.push_back( value.clone() );
    }
    return *this;
}

dawn::Value::Value( Bool value )
    : m_regref( bool_pool().new_register().cast<Void>() ), m_type( ValueType::BOOL )
{
    m_regref.cast<Bool>().value() = value;
}

dawn::Value::Value( Int value )
    : m_regref( int_pool().new_register().cast<Void>() ), m_type( ValueType::INT )
{
    m_regref.cast<Int>().value() = value;
}

dawn::Value::Value( Float value )
    : m_regref( float_pool().new_register().cast<Void>() ), m_type( ValueType::FLOAT )
{
    m_regref.cast<Float>().value() = value;
}

dawn::Value::Value( Char value )
    : m_regref( char_pool().new_register().cast<Void>() ), m_type( ValueType::CHAR )
{
    m_regref.cast<Char>().value() = value;
}

dawn::Value::Value( StringRef const& value )
    : m_regref( string_pool().new_register().cast<Void>() ), m_type( ValueType::STRING )
{
    m_regref.cast<String>().value() = value;
}

dawn::Value::Value( Function const& value )
    : m_regref( function_pool().new_register().cast<Void>() ), m_type( ValueType::FUNCTION )
{
    m_regref.cast<Function>().value() = value;
}

dawn::Value::Value( EnumValue const& value )
    : m_regref( enum_pool().new_register().cast<Void>() ), m_type( ValueType::ENUM )
{
    m_regref.cast<EnumValue>().value() = value;
}

dawn::Value::Value( StructValue const& value )
    : m_regref( struct_pool().new_register().cast<Void>() ), m_type( ValueType::STRUCT )
{
    m_regref.cast<StructValue>().value() = value;
}

dawn::Value::Value( ArrayValue const& value )
    : m_regref( array_pool().new_register().cast<Void>() ), m_type( ValueType::ARRAY )
{
    m_regref.cast<ArrayValue>().value() = value;
}

dawn::Value::Value( RangeValue const& value )
    : m_regref( range_pool().new_register().cast<Void>() ), m_type( ValueType::RANGE )
{
    m_regref.cast<RangeValue>().value() = value;
}

void dawn::Value::assign( Value const& other )
{
    if ( m_const )
        PANIC( "can't assign [", other.m_type, "] to a const value" );

    if ( m_type != other.m_type )
        PANIC( "can't assign [", other.m_type, "] to [", m_type, "]" );

    switch ( m_type )
    {
    case ValueType::BOOL:
        as_bool() = other.as_bool();
        break;

    case ValueType::INT:
        as_int() = other.as_int();
        break;

    case ValueType::FLOAT:
        as_float() = other.as_float();
        break;

    case ValueType::CHAR:
        as_char() = other.as_char();
        break;

    case ValueType::STRING:
        as_string() = other.as_string();
        break;

    case ValueType::FUNCTION:
        as_function() = other.as_function();
        break;

    case ValueType::ENUM:
        as_enum() = other.as_enum();
        break;

    case ValueType::STRUCT:
        as_struct() = other.as_struct();
        break;

    case ValueType::ARRAY:
        as_array() = other.as_array();
        break;

    case ValueType::RANGE:
        as_range() = other.as_range();
        break;

    default:
        PANIC( "assign to [", m_type, "] not supported" );
    }

    unlock_const();
}

dawn::Value dawn::Value::clone() const
{
    switch ( m_type )
    {
    case ValueType::NOTHING:
        return Value{};

    case ValueType::BOOL:
        return Value{ as_bool() };

    case ValueType::INT:
        return Value{ as_int() };

    case ValueType::FLOAT:
        return Value{ as_float() };

    case ValueType::CHAR:
        return Value{ as_char() };

    case ValueType::STRING:
        return Value{ as_string() };

    case ValueType::FUNCTION:
        return Value{ as_function() };

    case ValueType::ENUM:
        return Value{ as_enum() };

    case ValueType::STRUCT:
        return Value{ as_struct() };

    case ValueType::ARRAY:
        return Value{ as_array() };

    case ValueType::RANGE:
        return Value{ as_range() };

    default:
        PANIC( "can't clone type [", Int( m_type ), "]" );
    }
}

dawn::Bool dawn::Value::is_const() const
{
    return m_const;
}

dawn::Value& dawn::Value::unlock_const()
{
    m_const = false;
    if ( m_type == ValueType::STRUCT )
    {
        auto& value = as_struct();
        for ( auto& field : value.parent->fields )
            value.members.at( field.id ).unlock_const();
    }
    else if ( m_type == ValueType::ARRAY )
    {
        auto& value = as_array();
        for ( auto& entry : value.data )
            entry.unlock_const();
    }
    return *this;
}

dawn::Value dawn::Value::un_plus( Engine& engine ) const
{
    switch ( type() )
    {
    case ValueType::INT:
        return Value{ +as_int() };

    case ValueType::FLOAT:
        return Value{ +as_float() };

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        Function* op = left.get_method( __add, true );
        if ( !op )
            PANIC( "+ struct [", IDSystem::get( left.parent->id ), "] not supported" );

        Value args[1] = { *this };
        return engine.handle_func( *op, args, (Int) std::size( args ) );
    }

    default:
        PANIC( "+ [", type(), "] not supported" );
    }
}

dawn::Value dawn::Value::un_minus( Engine& engine ) const
{
    switch ( type() )
    {
    case ValueType::INT:
        return Value{ -as_int() };

    case ValueType::FLOAT:
        return Value{ -as_float() };

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        Function* op = left.get_method( __sub, true );
        if ( !op )
            PANIC( "- struct [", IDSystem::get( left.parent->id ), "] not supported" );

        Value args[1] = { *this };
        return engine.handle_func( *op, args, (Int) std::size( args ) );
    }

    default:
        PANIC( "- [", type(), "] not supported" );
    }
}

dawn::Value dawn::Value::op_add( Engine& engine, Value const& other ) const
{
    switch ( type() )
    {
    case ValueType::INT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ as_int() + other.as_int() };

        case ValueType::FLOAT:
            return Value{ as_int() + other.as_float() };

        default:
            PANIC( "[", type(), "] + [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ as_float() + other.as_int() };

        case ValueType::FLOAT:
            return Value{ as_float() + other.as_float() };

        default:
            PANIC( "[", type(), "] + [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRING:
    {
        switch ( other.type() )
        {
        case ValueType::STRING:
            return Value{ as_string() + other.as_string() };

        default:
            PANIC( "[", type(), "] + [", other.type(), "] not supported" );
        }
    }

    case ValueType::ARRAY:
    {
        switch ( other.type() )
        {
        case ValueType::ARRAY:
        {
            ArrayValue result;
            result.data.insert( result.data.end(), as_array().data.begin(), as_array().data.end() );
            result.data.insert( result.data.end(), other.as_array().data.begin(), other.as_array().data.end() );
            return Value{ result };
        }

        default:
            PANIC( "[", type(), "] + [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        Function* op = left.get_method( __add, false );
        if ( !op )
            PANIC( "struct [", IDSystem::get( left.parent->id ), "] + [", other.type(), "] not supported" );

        Value args[2] = { *this, other };
        return engine.handle_func( *op, args, (Int) std::size( args ) );
    }

    default:
        PANIC( "[", type(), "] + [", other.type(), "] not supported" );
    }
}

dawn::Value dawn::Value::op_sub( Engine& engine, Value const& other ) const
{
    switch ( type() )
    {
    case ValueType::INT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ as_int() - other.as_int() };

        case ValueType::FLOAT:
            return Value{ as_int() - other.as_float() };

        default:
            PANIC( "[", type(), "] - [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ as_float() - other.as_int() };

        case ValueType::FLOAT:
            return Value{ as_float() - other.as_float() };

        default:
            PANIC( "[", type(), "] - [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        Function* op = left.get_method( __sub, false );
        if ( !op )
            PANIC( "struct [", IDSystem::get( left.parent->id ), "] - [", other.type(), "] not supported" );

        Value args[2] = { *this, other };
        return engine.handle_func( *op, args, (Int) std::size( args ) );
    }

    default:
        PANIC( "[", type(), "] - [", other.type(), "] not supported" );
    }
}

dawn::Value dawn::Value::op_mul( Engine& engine, Value const& other ) const
{
    switch ( type() )
    {
    case ValueType::INT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ as_int() * other.as_int() };

        case ValueType::FLOAT:
            return Value{ as_int() * other.as_float() };

        default:
            PANIC( "[", type(), "] * [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ as_float() * other.as_int() };

        case ValueType::FLOAT:
            return Value{ as_float() * other.as_float() };

        default:
            PANIC( "[", type(), "] * [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        Function* op = left.get_method( __mul, false );
        if ( !op )
            PANIC( "struct [", IDSystem::get( left.parent->id ), "] * [", other.type(), "] not supported" );

        Value args[2] = { *this, other };
        return engine.handle_func( *op, args, (Int) std::size( args ) );
    }

    default:
        PANIC( "[", type(), "] * [", other.type(), "] not supported" );
    }
}

dawn::Value dawn::Value::op_div( Engine& engine, Value const& other ) const
{
    switch ( type() )
    {
    case ValueType::INT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ as_int() / other.as_int() };

        case ValueType::FLOAT:
            return Value{ as_int() / other.as_float() };

        default:
            PANIC( "[", type(), "] / [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ as_float() / other.as_int() };

        case ValueType::FLOAT:
            return Value{ as_float() / other.as_float() };

        default:
            PANIC( "[", type(), "] / [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        Function* op = left.get_method( __div, false );
        if ( !op )
            PANIC( "struct [", IDSystem::get( left.parent->id ), "] / [", other.type(), "] not supported" );

        Value args[2] = { *this, other };
        return engine.handle_func( *op, args, (Int) std::size( args ) );
    }

    default:
        PANIC( "[", type(), "] / [", other.type(), "] not supported" );
    }
}

dawn::Value dawn::Value::op_pow( Engine& engine, Value const& other ) const
{
    switch ( type() )
    {
    case ValueType::INT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ (Int) std::pow( as_int(), other.as_int() ) };

        case ValueType::FLOAT:
            return Value{ std::pow( as_int(), other.as_float() ) };

        default:
            PANIC( "[", type(), "] ^ [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ std::pow( as_float(), other.as_int() ) };

        case ValueType::FLOAT:
            return Value{ std::pow( as_float(), other.as_float() ) };

        default:
            PANIC( "[", type(), "] ^ [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        Function* op = left.get_method( __pow, false );
        if ( !op )
            PANIC( "struct [", IDSystem::get( left.parent->id ), "] ^ [", other.type(), "] not supported" );

        Value args[2] = { *this, other };
        return engine.handle_func( *op, args, (Int) std::size( args ) );
    }

    default:
        PANIC( "[", type(), "] ^ [", other.type(), "] not supported" );
    }
}

dawn::Value dawn::Value::op_mod( Engine& engine, Value const& other ) const
{
    switch ( type() )
    {
    case ValueType::INT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ as_int() % other.as_int() };

        case ValueType::FLOAT:
            return Value{ mymod( (Float) as_int(), other.as_float() ) };

        default:
            PANIC( "[", type(), "] % [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ mymod( as_float(), (Float) other.as_int() ) };

        case ValueType::FLOAT:
            return Value{ mymod( as_float(), other.as_float() ) };

        default:
            PANIC( "[", type(), "] % [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        Function* op = left.get_method( __mod, false );
        if ( !op )
            PANIC( "struct [", IDSystem::get( left.parent->id ), "] % [", other.type(), "] not supported" );

        Value args[2] = { *this, other };
        return engine.handle_func( *op, args, (Int) std::size( args ) );
    }

    default:
        PANIC( "[", type(), "] % [", other.type(), "] not supported" );
    }
}

dawn::Value dawn::Value::op_cmpr( Engine& engine, Value const& other ) const
{
    switch ( type() )
    {
    case ValueType::BOOL:
    {
        switch ( other.type() )
        {
        case ValueType::BOOL:
            return Value{ (Int) ( as_bool() <=> other.as_bool() )._Value };

        default:
            PANIC( "[", type(), "] <=> [", other.type(), "] not supported" );
        }
    }

    case ValueType::INT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ (Int) ( as_int() <=> other.as_int() )._Value };

        case ValueType::FLOAT:
            return Value{ (Int) ( as_int() <=> other.as_float() )._Value };

        default:
            PANIC( "[", type(), "] <=> [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ (Int) ( as_float() <=> other.as_int() )._Value };

        case ValueType::FLOAT:
            return Value{ (Int) ( as_float() <=> other.as_float() )._Value };

        default:
            PANIC( "[", type(), "] <=> [", other.type(), "] not supported" );
        }
    }

    case ValueType::CHAR:
    {
        switch ( other.type() )
        {
        case ValueType::CHAR:
            return Value{ (Int) ( as_char() <=> other.as_char() )._Value };

        default:
            PANIC( "[", type(), "] <=> [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRING:
    {
        switch ( other.type() )
        {
        case ValueType::STRING:
            return Value{ (Int) ( as_string() <=> other.as_string() )._Value };

        default:
            PANIC( "[", type(), "] <=> [", other.type(), "] not supported" );
        }
    }

    case ValueType::ENUM:
    {
        switch ( other.type() )
        {
        case ValueType::ENUM:
        {
            auto& left = as_enum();
            auto& right = other.as_enum();
            if ( left.parent != right.parent )
                PANIC( "enum [", IDSystem::get( left.parent->id ), "] <=> enum [", IDSystem::get( right.parent->id ), "] not supported" );

            return Value{ (Int) ( left.key_id <=> right.key_id )._Value };
        }

        default:
            PANIC( "[", type(), "] <=> [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        Function* op = left.get_method( __cmpr, false );
        if ( !op )
            PANIC( "struct [", IDSystem::get( left.parent->id ), "] <=> [", other.type(), "] not supported" );

        Value args[2] = { *this, other };
        return engine.handle_func( *op, args, (Int) std::size( args ) );
    }

    case ValueType::ARRAY:
    {
        switch ( other.type() )
        {
        case ValueType::ARRAY:
        {
            auto& left = as_array().data;
            auto& right = other.as_array().data;

            for ( Int i = 0; i < (Int) std::min( left.size(), right.size() ); i++ )
            {
                Int cmpr_res = left[i].op_cmpr( engine, right[i] ).as_int();
                if ( cmpr_res != 0 )
                    return Value{ cmpr_res };
            }
            return Value{ (Int) ( left.size() <=> right.size() )._Value };
        }

        default:
            PANIC( "[", type(), "] <=> [", other.type(), "] not supported" );
        }
    }

    default:
        PANIC( "[", type(), "] <=> [", other.type(), "] not supported" );
    }
}

dawn::Value dawn::Value::op_eq( Engine& engine, Value const& other ) const
{
    auto result = op_cmpr( engine, other ).as_int();
    return Value{ result == 0 };
}

dawn::Value dawn::Value::op_neq( Engine& engine, Value const& other ) const
{
    auto result = op_cmpr( engine, other ).as_int();
    return Value{ result != 0 };
}

dawn::Value dawn::Value::op_less( Engine& engine, Value const& other ) const
{
    auto result = op_cmpr( engine, other ).as_int();
    return Value{ result < 0 };
}

dawn::Value dawn::Value::op_great( Engine& engine, Value const& other ) const
{
    auto result = op_cmpr( engine, other ).as_int();
    return Value{ result > 0 };
}

dawn::Value dawn::Value::op_lesseq( Engine& engine, Value const& other ) const
{
    auto result = op_cmpr( engine, other ).as_int();
    return Value{ result <= 0 };
}

dawn::Value dawn::Value::op_greateq( Engine& engine, Value const& other ) const
{
    auto result = op_cmpr( engine, other ).as_int();
    return Value{ result >= 0 };
}

dawn::Value dawn::Value::un_not( Engine& engine ) const
{
    return Value{ !to_bool( engine ) };
}

dawn::Value dawn::Value::op_and( Engine& engine, Value const& other ) const
{
    return Value{ to_bool( engine ) && other.to_bool( engine ) };
}

dawn::Value dawn::Value::op_or( Engine& engine, Value const& other ) const
{
    return Value{ to_bool( engine ) || other.to_bool( engine ) };
}

dawn::Value dawn::Value::op_range( Engine& engine, Value const& other ) const
{
    RangeValue result;
    result.start_incl = to_int( engine );
    result.end_excl = other.to_int( engine );
    return Value{ result };
}

dawn::Bool dawn::Value::to_bool( Engine& engine ) const
{
    switch ( type() )
    {
    case ValueType::NOTHING:
        return Bool{};

    case ValueType::BOOL:
        return as_bool();

    case ValueType::INT:
        return (Bool) as_int();

    case ValueType::FLOAT:
        return (Bool) as_float();

    case ValueType::CHAR:
        return (Bool) as_char();

    case ValueType::STRING:
        return as_string() == kw_true;

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        Function* method = left.get_method( _to_bool, true );
        if ( !method )
            PANIC( "can't convert struct [", IDSystem::get( left.parent->id ), "] to bool" );

        Value args[1] = { *this };
        return engine.handle_func( *method, args, (Int) std::size( args ) ).to_bool( engine );
    }

    default:
        PANIC( "can't convert [", type(), "] to bool" );
    }
}

dawn::Int dawn::Value::to_int( Engine& engine ) const
{
    switch ( type() )
    {
    case ValueType::NOTHING:
        return Int{};

    case ValueType::BOOL:
        return (Int) as_bool();

    case ValueType::INT:
        return as_int();

    case ValueType::FLOAT:
        return (Int) as_float();

    case ValueType::CHAR:
        return (Int) as_char();

    case ValueType::STRING:
    {
        if ( auto optres = parse_int( as_string() ) )
            return *optres;
        throw Value{ dawn::format( "string \"", as_string(), "\" to int failed" ) };
    }

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        Function* method = left.get_method( _to_int, true );
        if ( !method )
            PANIC( "can't convert struct [", IDSystem::get( left.parent->id ), "] to int" );

        Value args[1] = { *this };
        return engine.handle_func( *method, args, (Int) std::size( args ) ).to_int( engine );
    }

    default:
        PANIC( "can't convert [", type(), "] to int" );
    }
}

dawn::Float dawn::Value::to_float( Engine& engine ) const
{
    switch ( type() )
    {
    case ValueType::NOTHING:
        return Float{};

    case ValueType::BOOL:
        return (Float) as_bool();

    case ValueType::INT:
        return (Float) as_int();

    case ValueType::FLOAT:
        return as_float();

    case ValueType::CHAR:
        return (Float) as_char();

    case ValueType::STRING:
    {
        if ( auto optres = parse_float( as_string() ) )
            return *optres;
        throw Value{ dawn::format( "string \"", as_string(), "\" to float failed" ) };
    }

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        Function* method = left.get_method( _to_float, true );
        if ( !method )
            PANIC( "can't convert struct [", IDSystem::get( left.parent->id ), "] to float" );

        Value args[1] = { *this };
        return engine.handle_func( *method, args, (Int) std::size( args ) ).to_float( engine );
    }

    default:
        PANIC( "can't convert [", type(), "] to float" );
    }
}

dawn::Char dawn::Value::to_char( Engine& engine ) const
{
    switch ( type() )
    {
    case ValueType::NOTHING:
        return Char{};

    case ValueType::BOOL:
        return as_bool() ? kw_true[0] : kw_false[0];

    case ValueType::INT:
        return (Char) as_int();

    case ValueType::FLOAT:
        return (Char) as_float();

    case ValueType::CHAR:
        return as_char();

    case ValueType::STRING:
        return as_string()[0];

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        Function* method = left.get_method( _to_char, true );
        if ( !method )
            PANIC( "can't convert struct [", IDSystem::get( left.parent->id ), "] to char" );

        Value args[1] = { *this };
        return engine.handle_func( *method, args, (Int) std::size( args ) ).to_char( engine );
    }

    default:
        PANIC( "can't convert [", type(), "] to char" );
    }
}

dawn::String dawn::Value::to_string( Engine& engine ) const
{
    switch ( type() )
    {
    case ValueType::NOTHING:
        return String{};

    case ValueType::BOOL:
        return String{ as_bool() ? kw_true : kw_false };

    case ValueType::INT:
        return std::to_string( as_int() );

    case ValueType::FLOAT:
    {
        String result = format( as_float() );
        if ( std::to_string( (Int) as_float() ) == result )
            result += sep_number;
        return result;
    }

    case ValueType::CHAR:
        return String( 1, as_char() );

    case ValueType::STRING:
        return as_string();

    case ValueType::FUNCTION:
    {
        StringStream stream;
        auto& func = as_function();

        if ( func.type() == FunctionType::LAMBDA )
            stream << "lambda" << op_lambda;
        else if ( func.type() == FunctionType::METHOD )
            stream << IDSystem::get( func.METHOD_self->as_struct().parent->id )
            << op_access << IDSystem::get( func.id ) << op_expr_opn;
        else
            stream << IDSystem::get( func.id ) << op_expr_opn;

        if ( !func.args.empty() )
        {
            for ( Int i = 0; i < (Int) func.args.size() - 1; i++ )
                stream << func.args[i].kind << ' ' << IDSystem::get( func.args[i].id ) << op_split << ' ';
            stream << func.args.back().kind << ' ' << IDSystem::get( func.args.back().id );
        }
        stream << ( func.type() == FunctionType::LAMBDA ? op_lambda : op_expr_cls );
        return stream.str();
    }

    case ValueType::ENUM:
    {
        auto& value = as_enum();
        return format( IDSystem::get( value.parent->id ), op_scope_opn, IDSystem::get( value.key_id ), op_scope_cls );
    }

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        if ( Function* method = left.get_method( _to_string, true ) )
        {
            Value args[1] = { *this };
            return engine.handle_func( *method, args, (Int) std::size( args ) ).to_string( engine );
        }
        else
        {
            StringStream stream;
            stream << IDSystem::get( left.parent->id );
            if ( left.members.empty() )
            {
                stream << op_scope_opn << op_scope_cls;
                return stream.str();
            }

            auto it = left.members.begin();
            stream << op_scope_opn;
            for ( ; it != --left.members.end(); ++it )
                stream << IDSystem::get( it->first ) << op_assign << it->second.to_string( engine ) << op_split << ' ';
            stream << IDSystem::get( it->first ) << op_assign << it->second.to_string( engine ) << op_scope_cls;
            return stream.str();
        }
    }

    case ValueType::ARRAY:
    {
        auto& value = as_array();
        if ( value.data.empty() )
            return format( op_array_opn, op_array_cls );

        StringStream stream;
        stream << op_array_opn;
        for ( Int i = 0; i < (Int) value.data.size() - 1; i++ )
            stream << value.data[i].to_string( engine ) << op_split << ' ';
        stream << value.data.back().to_string( engine ) << op_array_cls;
        return stream.str();
    }

    case ValueType::RANGE:
    {
        auto& value = as_range();
        return format( op_array_opn, value.start_incl, op_split, ' ', value.end_excl, op_expr_cls );
    }

    default:
        PANIC( "can't convert [", type(), "] to string" );
    }
}

std::ostream& dawn::operator<<( std::ostream& stream, ValueType type )
{
    switch ( type )
    {
    case ValueType::NOTHING: stream << tp_nothing; break;
    case ValueType::BOOL: stream << tp_bool; break;
    case ValueType::INT: stream << tp_int; break;
    case ValueType::FLOAT: stream << tp_float; break;
    case ValueType::CHAR: stream << tp_char; break;
    case ValueType::STRING: stream << tp_string; break;
    case ValueType::FUNCTION: stream << tp_function; break;
    case ValueType::ENUM: stream << tp_enum; break;
    case ValueType::STRUCT: stream << tp_struct; break;
    case ValueType::ARRAY: stream << tp_array; break;
    case ValueType::RANGE: stream << tp_range; break;
    }
    return stream;
}
