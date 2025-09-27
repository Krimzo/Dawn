#include "value.h"
#include "pool.h"
#include "engine.h"


#define ID_HELPER(id) static const Int _##id = IDSystem::get( #id )
#define OP_HELPER(op) static const Int __##op = IDSystem::get( op_##op )

namespace dawn
{
ID_HELPER( bool );
ID_HELPER( int );
ID_HELPER( float );
ID_HELPER( char );
ID_HELPER( string );

OP_HELPER( add );
OP_HELPER( sub );
OP_HELPER( mul );
OP_HELPER( div );
OP_HELPER( pow );
OP_HELPER( mod );
OP_HELPER( cmpr );
}

dawn::Bool dawn::FunctionValue::is_global() const
{
    return std::holds_alternative<AsGlobal>( data );
}

dawn::Bool dawn::FunctionValue::is_method() const
{
    return std::holds_alternative<AsMethod>( data );
}

dawn::Bool dawn::FunctionValue::is_lambda() const
{
    return std::holds_alternative<AsLambda>( data );
}

dawn::FunctionValue::AsGlobal& dawn::FunctionValue::as_global() const
{
    return const_cast<AsGlobal&>( std::get<AsGlobal>( data ) );
}

dawn::FunctionValue::AsMethod& dawn::FunctionValue::as_method() const
{
    return const_cast<AsMethod&>( std::get<AsMethod>( data ) );
}

dawn::FunctionValue::AsLambda& dawn::FunctionValue::as_lambda() const
{
    return const_cast<AsLambda&>( std::get<AsLambda>( data ) );
}

dawn::DFunction* dawn::FunctionValue::dfunction() const
{
    if ( std::holds_alternative<AsGlobal>( data ) )
    {
        auto& func = std::get<AsGlobal>( data ).func;
        if ( std::holds_alternative<DFunction>( func ) )
            return const_cast<DFunction*>( &std::get<DFunction>( func ) );
    }
    else if ( std::holds_alternative<AsMethod>( data ) )
    {
        auto& func = std::get<AsMethod>( data ).func;
        if ( std::holds_alternative<DFunction>( func ) )
            return const_cast<DFunction*>( &std::get<DFunction>( func ) );
    }
    else if ( std::holds_alternative<AsLambda>( data ) )
    {
        auto& func = std::get<AsLambda>( data ).func;
        if ( std::holds_alternative<DFunction>( func ) )
            return const_cast<DFunction*>( &std::get<DFunction>( func ) );
    }
    return nullptr;
}

dawn::CFunction* dawn::FunctionValue::cfunction() const
{
    if ( std::holds_alternative<AsGlobal>( data ) )
    {
        auto& func = std::get<AsGlobal>( data ).func;
        if ( std::holds_alternative<CFunction>( func ) )
            return const_cast<CFunction*>( &std::get<CFunction>( func ) );
    }
    else if ( std::holds_alternative<AsMethod>( data ) )
    {
        auto& func = std::get<AsMethod>( data ).func;
        if ( std::holds_alternative<CFunction>( func ) )
            return const_cast<CFunction*>( &std::get<CFunction>( func ) );
    }
    else if ( std::holds_alternative<AsLambda>( data ) )
    {
        auto& func = std::get<AsLambda>( data ).func;
        if ( std::holds_alternative<CFunction>( func ) )
            return const_cast<CFunction*>( &std::get<CFunction>( func ) );
    }
    return nullptr;
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

dawn::FunctionValue* dawn::StructValue::get_method( Int id, Bool has_no_args )
{
    auto it = members.find( id );
    if ( it == members.end() )
        return nullptr;

    if ( it->second.type() != ValueType::FUNCTION )
        return nullptr;

    auto& func = it->second.as_function();
    if ( !func.is_method() )
        return nullptr;

    if ( has_no_args )
    {
        auto* dfunc = func.dfunction();
        if ( !dfunc )
            return nullptr;
        if ( dfunc->args.size() != 1 )
            return nullptr;
    }
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

dawn::Value::Value( FunctionValue const& value )
    : m_regref( function_pool().new_register().cast<Void>() ), m_type( ValueType::FUNCTION )
{
    m_regref.cast<FunctionValue>().value() = value;
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

void dawn::Value::assign( Location const& location, Value const& other )
{
    if ( m_const )
        ENGINE_PANIC( location, "can not assign [", other.m_type, "] to a const value" );

    if ( m_type != other.m_type )
        ENGINE_PANIC( location, "can not assign [", other.m_type, "] to [", m_type, "]" );

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
        ENGINE_PANIC( location, "assign to [", m_type, "] not supported" );
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
    {
        Value result{ as_struct() };
        auto& struc = result.as_struct();
        auto& parent_methods = struc.parent->methods;
        for ( auto& [id, member] : struc.members )
        {
            if ( member.type() != ValueType::FUNCTION )
                continue;
            const auto it = std::find_if( parent_methods.begin(), parent_methods.end(), [&]( Function const& func )
                {
                    return func.id == id;
                } );
            if ( it != parent_methods.end() )
                *member.as_function().as_method().self = result;
        }
        return result;
    }

    case ValueType::ARRAY:
        return Value{ as_array() };

    case ValueType::RANGE:
        return Value{ as_range() };

    default:
        ENGINE_PANIC( Location{ Bad{} }, "can not clone type [", Int( m_type ), "]" );
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

dawn::Value dawn::Value::un_plus( Engine& engine, Location const& location ) const
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
        auto* op = left.get_method( __add, true );
        if ( !op )
            ENGINE_PANIC( location, dawn::op_add, " struct [", IDSystem::get( left.parent->id ), "] not supported" );

        Value args[1] = { *this };
        return engine.handle_func( location, *op, args, (Int) std::size( args ) );
    }

    default:
        ENGINE_PANIC( location, dawn::op_add, " [", type(), "] not supported" );
    }
}

dawn::Value dawn::Value::un_minus( Engine& engine, Location const& location ) const
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
        auto* op = left.get_method( __sub, true );
        if ( !op )
            ENGINE_PANIC( location, dawn::op_sub, " struct [", IDSystem::get( left.parent->id ), "] not supported" );

        Value args[1] = { *this };
        return engine.handle_func( location, *op, args, (Int) std::size( args ) );
    }

    default:
        ENGINE_PANIC( location, dawn::op_sub, " [", type(), "] not supported" );
    }
}

dawn::Value dawn::Value::op_add( Engine& engine, Location const& location, Value const& other ) const
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
            ENGINE_PANIC( location, "[", type(), "] ", dawn::op_add, " [", other.type(), "] not supported" );
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
            ENGINE_PANIC( location, "[", type(), "] ", dawn::op_add, " [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRING:
    {
        switch ( other.type() )
        {
        case ValueType::STRING:
            return Value{ as_string() + other.as_string() };

        default:
            ENGINE_PANIC( location, "[", type(), "] ", dawn::op_add, " [", other.type(), "] not supported" );
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
            ENGINE_PANIC( location, "[", type(), "] ", dawn::op_add, " [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        auto* op = left.get_method( __add, false );
        if ( !op )
            ENGINE_PANIC( location, "struct [", IDSystem::get( left.parent->id ), "] ", dawn::op_add, " [", other.type(), "] not supported" );

        Value args[2] = { *this, other };
        return engine.handle_func( location, *op, args, (Int) std::size( args ) );
    }

    default:
        ENGINE_PANIC( location, "[", type(), "] ", dawn::op_add, " [", other.type(), "] not supported" );
    }
}

dawn::Value dawn::Value::op_sub( Engine& engine, Location const& location, Value const& other ) const
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
            ENGINE_PANIC( location, "[", type(), "] ", dawn::op_sub, " [", other.type(), "] not supported" );
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
            ENGINE_PANIC( location, "[", type(), "] ", dawn::op_sub, " [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        auto* op = left.get_method( __sub, false );
        if ( !op )
            ENGINE_PANIC( location, "struct [", IDSystem::get( left.parent->id ), "] ", dawn::op_sub, " [", other.type(), "] not supported" );

        Value args[2] = { *this, other };
        return engine.handle_func( location, *op, args, (Int) std::size( args ) );
    }

    default:
        ENGINE_PANIC( location, "[", type(), "] ", dawn::op_sub, " [", other.type(), "] not supported" );
    }
}

dawn::Value dawn::Value::op_mul( Engine& engine, Location const& location, Value const& other ) const
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
            ENGINE_PANIC( location, "[", type(), "] ", dawn::op_mul, " [", other.type(), "] not supported" );
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
            ENGINE_PANIC( location, "[", type(), "] ", dawn::op_mul, " [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        auto* op = left.get_method( __mul, false );
        if ( !op )
            ENGINE_PANIC( location, "struct [", IDSystem::get( left.parent->id ), "] ", dawn::op_mul, " [", other.type(), "] not supported" );

        Value args[2] = { *this, other };
        return engine.handle_func( location, *op, args, (Int) std::size( args ) );
    }

    default:
        ENGINE_PANIC( location, "[", type(), "] ", dawn::op_mul, " [", other.type(), "] not supported" );
    }
}

dawn::Value dawn::Value::op_div( Engine& engine, Location const& location, Value const& other ) const
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
            ENGINE_PANIC( location, "[", type(), "] ", dawn::op_div, " [", other.type(), "] not supported" );
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
            ENGINE_PANIC( location, "[", type(), "] ", dawn::op_div, " [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        auto* op = left.get_method( __div, false );
        if ( !op )
            ENGINE_PANIC( location, "struct [", IDSystem::get( left.parent->id ), "] ", dawn::op_div, " [", other.type(), "] not supported" );

        Value args[2] = { *this, other };
        return engine.handle_func( location, *op, args, (Int) std::size( args ) );
    }

    default:
        ENGINE_PANIC( location, "[", type(), "] ", dawn::op_div, " [", other.type(), "] not supported" );
    }
}

dawn::Value dawn::Value::op_pow( Engine& engine, Location const& location, Value const& other ) const
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
            ENGINE_PANIC( location, "[", type(), "] ", dawn::op_pow, " [", other.type(), "] not supported" );
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
            ENGINE_PANIC( location, "[", type(), "] ", dawn::op_pow, " [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        auto* op = left.get_method( __pow, false );
        if ( !op )
            ENGINE_PANIC( location, "struct [", IDSystem::get( left.parent->id ), "] ", dawn::op_pow, " [", other.type(), "] not supported" );

        Value args[2] = { *this, other };
        return engine.handle_func( location, *op, args, (Int) std::size( args ) );
    }

    default:
        ENGINE_PANIC( location, "[", type(), "] ", dawn::op_pow, " [", other.type(), "] not supported" );
    }
}

dawn::Value dawn::Value::op_mod( Engine& engine, Location const& location, Value const& other ) const
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
            return Value{ dawn_mod( (Float) as_int(), other.as_float() ) };

        default:
            ENGINE_PANIC( location, "[", type(), "] ", dawn::op_mod, " [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ dawn_mod( as_float(), (Float) other.as_int() ) };

        case ValueType::FLOAT:
            return Value{ dawn_mod( as_float(), other.as_float() ) };

        default:
            ENGINE_PANIC( location, "[", type(), "] ", dawn::op_mod, " [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        auto* op = left.get_method( __mod, false );
        if ( !op )
            ENGINE_PANIC( location, "struct [", IDSystem::get( left.parent->id ), "] ", dawn::op_mod, " [", other.type(), "] not supported" );

        Value args[2] = { *this, other };
        return engine.handle_func( location, *op, args, (Int) std::size( args ) );
    }

    default:
        ENGINE_PANIC( location, "[", type(), "] ", dawn::op_mod, " [", other.type(), "] not supported" );
    }
}

dawn::Value dawn::Value::op_cmpr( Engine& engine, Location const& location, Value const& other ) const
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
            ENGINE_PANIC( location, "[", type(), "] ", dawn::op_cmpr, " [", other.type(), "] not supported" );
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
            ENGINE_PANIC( location, "[", type(), "] ", dawn::op_cmpr, " [", other.type(), "] not supported" );
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
            ENGINE_PANIC( location, "[", type(), "] ", dawn::op_cmpr, " [", other.type(), "] not supported" );
        }
    }

    case ValueType::CHAR:
    {
        switch ( other.type() )
        {
        case ValueType::CHAR:
            return Value{ (Int) ( as_char() <=> other.as_char() )._Value };

        default:
            ENGINE_PANIC( location, "[", type(), "] ", dawn::op_cmpr, " [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRING:
    {
        switch ( other.type() )
        {
        case ValueType::STRING:
            return Value{ (Int) ( as_string() <=> other.as_string() )._Value };

        default:
            ENGINE_PANIC( location, "[", type(), "] ", dawn::op_cmpr, " [", other.type(), "] not supported" );
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
                ENGINE_PANIC( location, "enum [", IDSystem::get( left.parent->id ), "] ", dawn::op_cmpr, " enum [", IDSystem::get( right.parent->id ), "] not supported" );

            return Value{ (Int) ( left.key_id <=> right.key_id )._Value };
        }

        default:
            ENGINE_PANIC( location, "[", type(), "] ", dawn::op_cmpr, " [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        auto* op = left.get_method( __cmpr, false );
        if ( !op )
            ENGINE_PANIC( location, "struct [", IDSystem::get( left.parent->id ), "] ", dawn::op_cmpr, " [", other.type(), "] not supported" );

        Value args[2] = { *this, other };
        return engine.handle_func( location, *op, args, (Int) std::size( args ) );
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
                Int cmpr_res = left[i].op_cmpr( engine, location, right[i] ).as_int();
                if ( cmpr_res != 0 )
                    return Value{ cmpr_res };
            }
            return Value{ (Int) ( left.size() <=> right.size() )._Value };
        }

        default:
            ENGINE_PANIC( location, "[", type(), "] ", dawn::op_cmpr, " [", other.type(), "] not supported" );
        }
    }

    default:
        ENGINE_PANIC( location, "[", type(), "] ", dawn::op_cmpr, " [", other.type(), "] not supported" );
    }
}

dawn::Value dawn::Value::op_eq( Engine& engine, Location const& location, Value const& other ) const
{
    auto result = op_cmpr( engine, location, other ).as_int();
    return Value{ result == 0 };
}

dawn::Value dawn::Value::op_neq( Engine& engine, Location const& location, Value const& other ) const
{
    auto result = op_cmpr( engine, location, other ).as_int();
    return Value{ result != 0 };
}

dawn::Value dawn::Value::op_less( Engine& engine, Location const& location, Value const& other ) const
{
    auto result = op_cmpr( engine, location, other ).as_int();
    return Value{ result < 0 };
}

dawn::Value dawn::Value::op_great( Engine& engine, Location const& location, Value const& other ) const
{
    auto result = op_cmpr( engine, location, other ).as_int();
    return Value{ result > 0 };
}

dawn::Value dawn::Value::op_lesseq( Engine& engine, Location const& location, Value const& other ) const
{
    auto result = op_cmpr( engine, location, other ).as_int();
    return Value{ result <= 0 };
}

dawn::Value dawn::Value::op_greateq( Engine& engine, Location const& location, Value const& other ) const
{
    auto result = op_cmpr( engine, location, other ).as_int();
    return Value{ result >= 0 };
}

dawn::Value dawn::Value::un_not( Engine& engine, Location const& location ) const
{
    return Value{ !to_bool( engine, location ) };
}

dawn::Value dawn::Value::op_and( Engine& engine, Location const& location, Value const& other ) const
{
    return Value{ to_bool( engine, location ) && other.to_bool( engine, location ) };
}

dawn::Value dawn::Value::op_or( Engine& engine, Location const& location, Value const& other ) const
{
    return Value{ to_bool( engine, location ) || other.to_bool( engine, location ) };
}

dawn::Value dawn::Value::op_range( Engine& engine, Location const& location, Value const& other ) const
{
    RangeValue result;
    result.start_incl = to_int( engine, location );
    result.end_excl = other.to_int( engine, location );
    return Value{ result };
}

dawn::Bool dawn::Value::to_bool( Engine& engine, Location const& location ) const
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
        return as_char() == kw_true.front();

    case ValueType::STRING:
        return as_string() == kw_true;

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        auto* method = left.get_method( _bool, true );
        if ( !method )
            ENGINE_PANIC( location, "can not convert struct [", IDSystem::get( left.parent->id ), "] to bool" );

        Value args[1] = { *this };
        return engine.handle_func( location, *method, args, (Int) std::size( args ) ).to_bool( engine, location );
    }

    default:
        ENGINE_PANIC( location, "can not convert [", type(), "] to bool" );
    }
}

dawn::Int dawn::Value::to_int( Engine& engine, Location const& location ) const
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
        auto* method = left.get_method( _int, true );
        if ( !method )
            ENGINE_PANIC( location, "can not convert struct [", IDSystem::get( left.parent->id ), "] to int" );

        Value args[1] = { *this };
        return engine.handle_func( location, *method, args, (Int) std::size( args ) ).to_int( engine, location );
    }

    default:
        ENGINE_PANIC( location, "can not convert [", type(), "] to int" );
    }
}

dawn::Float dawn::Value::to_float( Engine& engine, Location const& location ) const
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
        auto* method = left.get_method( _float, true );
        if ( !method )
            ENGINE_PANIC( location, "can not convert struct [", IDSystem::get( left.parent->id ), "] to float" );

        Value args[1] = { *this };
        return engine.handle_func( location, *method, args, (Int) std::size( args ) ).to_float( engine, location );
    }

    default:
        ENGINE_PANIC( location, "can not convert [", type(), "] to float" );
    }
}

dawn::Char dawn::Value::to_char( Engine& engine, Location const& location ) const
{
    switch ( type() )
    {
    case ValueType::NOTHING:
        return Char{};

    case ValueType::BOOL:
        return as_bool() ? kw_true.front() : kw_false.front();

    case ValueType::INT:
        return (Char) as_int();

    case ValueType::FLOAT:
        return (Char) as_float();

    case ValueType::CHAR:
        return as_char();

    case ValueType::STRING:
    {
        auto& str = as_string();
        if ( str.empty() )
            return Char{};
        return str.front();
    }

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        auto* method = left.get_method( _char, true );
        if ( !method )
            ENGINE_PANIC( location, "can not convert struct [", IDSystem::get( left.parent->id ), "] to char" );

        Value args[1] = { *this };
        return engine.handle_func( location, *method, args, (Int) std::size( args ) ).to_char( engine, location );
    }

    default:
        ENGINE_PANIC( location, "can not convert [", type(), "] to char" );
    }
}

dawn::String dawn::Value::to_string( Engine& engine, Location const& location ) const
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
        return format( std::showpoint, as_float() );

    case ValueType::CHAR:
        return String{ as_char() };

    case ValueType::STRING:
        return as_string();

    case ValueType::FUNCTION:
    {
        StringStream stream;
        auto& func = as_function();

        if ( func.is_global() )
        {
            auto& global = func.as_global();
            stream << IDSystem::get( global.id ) << op_expr_opn;
        }
        else if ( func.is_method() )
        {
            auto& method = func.as_method();
            stream << IDSystem::get( method.self->as_struct().parent->id )
                << op_access << IDSystem::get( method.id ) << op_expr_opn;
        }
        else
        {
            stream << "lambda" << op_lambda;
        }

        if ( auto* dfunc = func.dfunction() )
        {
            if ( !dfunc->args.empty() )
            {
                for ( Int i = 0; i < (Int) dfunc->args.size() - 1; i++ )
                    stream << dfunc->args[i].kind << ' ' << IDSystem::get( dfunc->args[i].id ) << op_split << ' ';
                stream << dfunc->args.back().kind << ' ' << IDSystem::get( dfunc->args.back().id );
            }
        }

        stream << ( func.is_lambda() ? op_lambda : op_expr_cls );
        return stream.str();
    }

    case ValueType::ENUM:
    {
        auto const& value = as_enum();
        return format( IDSystem::get( value.parent->id ), op_link, IDSystem::get( value.key_id ) );
    }

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        if ( auto* method = left.get_method( _string, true ) )
        {
            Value args[1] = { *this };
            return engine.handle_func( location, *method, args, (Int) std::size( args ) ).to_string( engine, location );
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
                stream << IDSystem::get( it->first ) << op_assign << it->second.to_string( engine, location ) << op_split << ' ';
            stream << IDSystem::get( it->first ) << op_assign << it->second.to_string( engine, location ) << op_scope_cls;
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
            stream << value.data[i].to_string( engine, location ) << op_split << ' ';
        stream << value.data.back().to_string( engine, location ) << op_array_cls;
        return stream.str();
    }

    case ValueType::RANGE:
    {
        auto& value = as_range();
        return format( op_array_opn, value.start_incl, op_split, ' ', value.end_excl, op_expr_cls );
    }

    default:
        ENGINE_PANIC( location, "can not convert [", type(), "] to string" );
    }
}
