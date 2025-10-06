#include "value.h"
#include "pool.h"
#include "engine.h"


#define ID_HELPER(id) static const ID _##id = IDSystem::get( #id )
#define OP_HELPER(op) static const ID __##op = IDSystem::get( op_##op )

namespace dawn
{
ID_HELPER( void );
ID_HELPER( bool );
ID_HELPER( int );
ID_HELPER( float );
ID_HELPER( char );
ID_HELPER( string );
ID_HELPER( function );
ID_HELPER( array );
ID_HELPER( range );

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

dawn::StructValue::StructValue( StructValue const& other )
    : parent_id( other.parent_id )
{
    fields.reserve( other.fields.size() );
    for ( auto& [key, val] : other.fields )
        fields[key] = val.clone();
    methods.reserve( other.methods.size() );
    for ( auto& [key, val] : other.methods )
        methods[key] = val.clone();
}

dawn::StructValue& dawn::StructValue::operator=( StructValue const& other )
{
    if ( this != &other )
    {
        parent_id = other.parent_id;
        fields.clear();
        fields.reserve( other.fields.size() );
        for ( auto& [key, val] : other.fields )
            fields[key] = val.clone();
        methods.clear();
        methods.reserve( other.methods.size() );
        for ( auto& [key, val] : other.methods )
            methods[key] = val.clone();
    }
    return *this;
}

dawn::StructValue::StructValue( StructValue&& other ) noexcept
    : parent_id( std::move( other.parent_id ) )
    , fields( std::move( other.fields ) )
    , methods( std::move( other.methods ) )
{
}

dawn::StructValue& dawn::StructValue::operator=( StructValue&& other ) noexcept
{
    if ( this != &other )
    {
        parent_id = std::move( other.parent_id );
        fields = std::move( other.fields );
        methods = std::move( other.methods );
    }
    return *this;
}

dawn::FunctionValue* dawn::StructValue::get_method( ID id, Bool has_no_args )
{
    auto it = methods.find( id );
    if ( it == methods.end() )
        return nullptr;
    return &it->second.as_function();
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

dawn::ArrayValue::ArrayValue( ArrayValue&& other ) noexcept
    : data( std::move( other.data ) )
{
}

dawn::ArrayValue& dawn::ArrayValue::operator=( ArrayValue&& other ) noexcept
{
    if ( this != &other )
        data = std::move( other.data );
    return *this;
}

dawn::Value::Value( Bool value, Location const& location )
    : m_regref( bool_pool().new_register().as<ValueInfo>() )
{
    auto& storage = *m_regref.as<ValueStorage<Bool>>();
    storage.info.location = location;
    storage.info.type = ValueType::BOOL;
    storage.info.is_const = true;
    storage.value = value;
}

dawn::Value::Value( Int value, Location const& location )
    : m_regref( int_pool().new_register().as<ValueInfo>() )
{
    auto& storage = *m_regref.as<ValueStorage<Int>>();
    storage.info.location = location;
    storage.info.type = ValueType::INT;
    storage.info.is_const = true;
    storage.value = value;
}

dawn::Value::Value( Float value, Location const& location )
    : m_regref( float_pool().new_register().as<ValueInfo>() )
{
    auto& storage = *m_regref.as<ValueStorage<Float>>();
    storage.info.location = location;
    storage.info.type = ValueType::FLOAT;
    storage.info.is_const = true;
    storage.value = value;
}

dawn::Value::Value( Char value, Location const& location )
    : m_regref( char_pool().new_register().as<ValueInfo>() )
{
    auto& storage = *m_regref.as<ValueStorage<Char>>();
    storage.info.location = location;
    storage.info.type = ValueType::CHAR;
    storage.info.is_const = true;
    storage.value = value;
}

dawn::Value::Value( StringRef const& value, Location const& location )
    : m_regref( string_pool().new_register().as<ValueInfo>() )
{
    auto& storage = *m_regref.as<ValueStorage<String>>();
    storage.info.location = location;
    storage.info.type = ValueType::STRING;
    storage.info.is_const = true;
    storage.value = value;
}

dawn::Value::Value( FunctionValue const& value, Location const& location )
    : m_regref( function_pool().new_register().as<ValueInfo>() )
{
    auto& storage = *m_regref.as<ValueStorage<FunctionValue>>();
    storage.info.location = location;
    storage.info.type = ValueType::FUNCTION;
    storage.info.is_const = true;
    storage.value = value;
}

dawn::Value::Value( EnumValue const& value, Location const& location )
    : m_regref( enum_pool().new_register().as<ValueInfo>() )
{
    auto& storage = *m_regref.as<ValueStorage<EnumValue>>();
    storage.info.location = location;
    storage.info.type = ValueType::ENUM;
    storage.info.is_const = true;
    storage.value = value;
}

dawn::Value::Value( StructValue const& value, Location const& location )
    : m_regref( struct_pool().new_register().as<ValueInfo>() )
{
    auto& storage = *m_regref.as<ValueStorage<StructValue>>();
    storage.info.location = location;
    storage.info.type = ValueType::STRUCT;
    storage.info.is_const = true;
    storage.value = value;
}

dawn::Value::Value( ArrayValue const& value, Location const& location )
    : m_regref( array_pool().new_register().as<ValueInfo>() )
{
    auto& storage = *m_regref.as<ValueStorage<ArrayValue>>();
    storage.info.location = location;
    storage.info.type = ValueType::ARRAY;
    storage.info.is_const = true;
    storage.value = value;
}

dawn::Value::Value( RangeValue const& value, Location const& location )
    : m_regref( range_pool().new_register().as<ValueInfo>() )
{
    auto& storage = *m_regref.as<ValueStorage<RangeValue>>();
    storage.info.location = location;
    storage.info.type = ValueType::RANGE;
    storage.info.is_const = true;
    storage.value = value;
}

dawn::Bool& dawn::Value::as_bool() const
{
    if ( type() != ValueType::BOOL )
        ENGINE_PANIC( location(), "expected [", ValueType::BOOL, "] but got [", type(), "]" );
    return m_regref.as<ValueStorage<Bool>>()->value;
}

dawn::Int& dawn::Value::as_int() const
{
    if ( type() != ValueType::INT )
        ENGINE_PANIC( location(), "expected [", ValueType::INT, "] but got [", type(), "]" );
    return m_regref.as<ValueStorage<Int>>()->value;
}

dawn::Float& dawn::Value::as_float() const
{
    if ( type() != ValueType::FLOAT )
        ENGINE_PANIC( location(), "expected [", ValueType::FLOAT, "] but got [", type(), "]" );
    return m_regref.as<ValueStorage<Float>>()->value;
}

dawn::Char& dawn::Value::as_char() const
{
    if ( type() != ValueType::CHAR )
        ENGINE_PANIC( location(), "expected [", ValueType::CHAR, "] but got [", type(), "]" );
    return m_regref.as<ValueStorage<Char>>()->value;
}

dawn::String& dawn::Value::as_string() const
{
    if ( type() != ValueType::STRING )
        ENGINE_PANIC( location(), "expected [", ValueType::STRING, "] but got [", type(), "]" );
    return m_regref.as<ValueStorage<String>>()->value;
}

dawn::FunctionValue& dawn::Value::as_function() const
{
    if ( type() != ValueType::FUNCTION )
        ENGINE_PANIC( location(), "expected [", ValueType::FUNCTION, "] but got [", type(), "]" );
    return m_regref.as<ValueStorage<FunctionValue>>()->value;
}

dawn::EnumValue& dawn::Value::as_enum() const
{
    if ( type() != ValueType::ENUM )
        ENGINE_PANIC( location(), "expected [", ValueType::ENUM, "] but got [", type(), "]" );
    return m_regref.as<ValueStorage<EnumValue>>()->value;
}

dawn::StructValue& dawn::Value::as_struct() const
{
    if ( type() != ValueType::STRUCT )
        ENGINE_PANIC( location(), "expected [", ValueType::STRUCT, "] but got [", type(), "]" );
    return m_regref.as<ValueStorage<StructValue>>()->value;
}

dawn::ArrayValue& dawn::Value::as_array() const
{
    if ( type() != ValueType::ARRAY )
        ENGINE_PANIC( location(), "expected [", ValueType::ARRAY, "] but got [", type(), "]" );
    return m_regref.as<ValueStorage<ArrayValue>>()->value;
}

dawn::RangeValue& dawn::Value::as_range() const
{
    if ( type() != ValueType::RANGE )
        ENGINE_PANIC( location(), "expected [", ValueType::RANGE, "] but got [", type(), "]" );
    return m_regref.as<ValueStorage<RangeValue>>()->value;
}

dawn::Location const& dawn::Value::location() const
{
    return m_regref ? m_regref->location : LOCATION_NONE;
}

dawn::ValueType dawn::Value::type() const
{
    return m_regref ? m_regref->type : ValueType::VOID;
}

dawn::ID dawn::Value::type_id() const
{
    switch ( type() )
    {
    case ValueType::VOID:
        return _void;

    case ValueType::BOOL:
        return _bool;

    case ValueType::INT:
        return _int;

    case ValueType::FLOAT:
        return _float;

    case ValueType::CHAR:
        return _char;

    case ValueType::STRING:
        return _string;

    case ValueType::FUNCTION:
        return _function;

    case ValueType::ENUM:
        return as_enum().parent_id;

    case ValueType::STRUCT:
        return as_struct().parent_id;

    case ValueType::ARRAY:
        return _array;

    case ValueType::RANGE:
        return _range;

    default:
        return ID{};
    }
}

void dawn::Value::assign( Value const& other )
{
    if ( is_const() )
        ENGINE_PANIC( location(), "can not assign [", IDSystem::get( other.type_id() ), "] to a const value" );

    if ( type_id() != other.type_id() )
        ENGINE_PANIC( location(), "can not assign [", IDSystem::get( other.type_id() ), "] to [", IDSystem::get( type_id() ), "]" );

    switch ( type() )
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
        ENGINE_PANIC( location(), "assign to [", type(), "] not supported" );
    }

    unlock_const();
}

dawn::Value dawn::Value::clone() const
{
    switch ( type() )
    {
    case ValueType::VOID:
        return Value{};

    case ValueType::BOOL:
        return Value{ as_bool(), location() };

    case ValueType::INT:
        return Value{ as_int(), location() };

    case ValueType::FLOAT:
        return Value{ as_float(), location() };

    case ValueType::CHAR:
        return Value{ as_char(), location() };

    case ValueType::STRING:
        return Value{ as_string(), location() };

    case ValueType::FUNCTION:
        return Value{ as_function(), location() };

    case ValueType::ENUM:
        return Value{ as_enum(), location() };

    case ValueType::STRUCT:
    {
        Value result{ as_struct(), location() };
        for ( auto& [_, method] : result.as_struct().methods )
            *method.as_function().as_method().self = result;
        return result;
    }

    case ValueType::ARRAY:
        return Value{ as_array(), location() };

    case ValueType::RANGE:
        return Value{ as_range(), location() };

    default:
        ENGINE_PANIC( LOCATION_NONE, "can not clone type [", (Int) type(), "]" );
    }
}

dawn::Bool dawn::Value::is_const() const
{
    return m_regref ? m_regref->is_const : true;
}

dawn::Value& dawn::Value::unlock_const()
{
    if ( m_regref )
        m_regref->is_const = false;
    switch ( type() )
    {
    case ValueType::STRUCT:
    {
        auto& value = as_struct();
        for ( auto& [_, field] : value.fields )
            field.unlock_const();
    }
    break;

    case ValueType::ARRAY:
    {
        auto& value = as_array();
        for ( auto& entry : value.data )
            entry.unlock_const();
    }
    break;
    }
    return *this;
}

dawn::Value dawn::Value::un_plus( Engine& engine ) const
{
    switch ( type() )
    {
    case ValueType::INT:
        return Value{ +as_int(), location() };

    case ValueType::FLOAT:
        return Value{ +as_float(), location() };

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        auto* op = left.get_method( __add, true );
        if ( !op )
            ENGINE_PANIC( location(), dawn::op_add, " struct [", IDSystem::get( left.parent_id ), "] not supported" );

        Value args[1] = { *this };
        return engine.handle_func( location(), *op, args, (Int) std::size( args ) );
    }

    default:
        ENGINE_PANIC( location(), dawn::op_add, " [", type(), "] not supported" );
    }
}

dawn::Value dawn::Value::un_minus( Engine& engine ) const
{
    switch ( type() )
    {
    case ValueType::INT:
        return Value{ -as_int(), location() };

    case ValueType::FLOAT:
        return Value{ -as_float(), location() };

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        auto* op = left.get_method( __sub, true );
        if ( !op )
            ENGINE_PANIC( location(), dawn::op_sub, " struct [", IDSystem::get( left.parent_id ), "] not supported" );

        Value args[1] = { *this };
        return engine.handle_func( location(), *op, args, (Int) std::size( args ) );
    }

    default:
        ENGINE_PANIC( location(), dawn::op_sub, " [", type(), "] not supported" );
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
            return Value{ as_int() + other.as_int(), location() };

        case ValueType::FLOAT:
            return Value{ as_int() + other.as_float(), location() };

        default:
            ENGINE_PANIC( location(), "[", type(), "] ", dawn::op_add, " [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ as_float() + other.as_int(), location() };

        case ValueType::FLOAT:
            return Value{ as_float() + other.as_float(), location() };

        default:
            ENGINE_PANIC( location(), "[", type(), "] ", dawn::op_add, " [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRING:
    {
        switch ( other.type() )
        {
        case ValueType::STRING:
            return Value{ as_string() + other.as_string(), location() };

        default:
            ENGINE_PANIC( location(), "[", type(), "] ", dawn::op_add, " [", other.type(), "] not supported" );
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
            return Value{ result, location() };
        }

        default:
            ENGINE_PANIC( location(), "[", type(), "] ", dawn::op_add, " [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        auto* op = left.get_method( __add, false );
        if ( !op )
            ENGINE_PANIC( location(), "struct [", IDSystem::get( left.parent_id ), "] ", dawn::op_add, " [", other.type(), "] not supported" );

        Value args[2] = { *this, other };
        return engine.handle_func( location(), *op, args, (Int) std::size( args ) );
    }

    default:
        ENGINE_PANIC( location(), "[", type(), "] ", dawn::op_add, " [", other.type(), "] not supported" );
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
            return Value{ as_int() - other.as_int(), location() };

        case ValueType::FLOAT:
            return Value{ as_int() - other.as_float(), location() };

        default:
            ENGINE_PANIC( location(), "[", type(), "] ", dawn::op_sub, " [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ as_float() - other.as_int(), location() };

        case ValueType::FLOAT:
            return Value{ as_float() - other.as_float(), location() };

        default:
            ENGINE_PANIC( location(), "[", type(), "] ", dawn::op_sub, " [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        auto* op = left.get_method( __sub, false );
        if ( !op )
            ENGINE_PANIC( location(), "struct [", IDSystem::get( left.parent_id ), "] ", dawn::op_sub, " [", other.type(), "] not supported" );

        Value args[2] = { *this, other };
        return engine.handle_func( location(), *op, args, (Int) std::size( args ) );
    }

    default:
        ENGINE_PANIC( location(), "[", type(), "] ", dawn::op_sub, " [", other.type(), "] not supported" );
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
            return Value{ as_int() * other.as_int(), location() };

        case ValueType::FLOAT:
            return Value{ as_int() * other.as_float(), location() };

        default:
            ENGINE_PANIC( location(), "[", type(), "] ", dawn::op_mul, " [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ as_float() * other.as_int(), location() };

        case ValueType::FLOAT:
            return Value{ as_float() * other.as_float(), location() };

        default:
            ENGINE_PANIC( location(), "[", type(), "] ", dawn::op_mul, " [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        auto* op = left.get_method( __mul, false );
        if ( !op )
            ENGINE_PANIC( location(), "struct [", IDSystem::get( left.parent_id ), "] ", dawn::op_mul, " [", other.type(), "] not supported" );

        Value args[2] = { *this, other };
        return engine.handle_func( location(), *op, args, (Int) std::size( args ) );
    }

    default:
        ENGINE_PANIC( location(), "[", type(), "] ", dawn::op_mul, " [", other.type(), "] not supported" );
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
            return Value{ as_int() / other.as_int(), location() };

        case ValueType::FLOAT:
            return Value{ as_int() / other.as_float(), location() };

        default:
            ENGINE_PANIC( location(), "[", type(), "] ", dawn::op_div, " [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ as_float() / other.as_int(), location() };

        case ValueType::FLOAT:
            return Value{ as_float() / other.as_float(), location() };

        default:
            ENGINE_PANIC( location(), "[", type(), "] ", dawn::op_div, " [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        auto* op = left.get_method( __div, false );
        if ( !op )
            ENGINE_PANIC( location(), "struct [", IDSystem::get( left.parent_id ), "] ", dawn::op_div, " [", other.type(), "] not supported" );

        Value args[2] = { *this, other };
        return engine.handle_func( location(), *op, args, (Int) std::size( args ) );
    }

    default:
        ENGINE_PANIC( location(), "[", type(), "] ", dawn::op_div, " [", other.type(), "] not supported" );
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
            return Value{ (Int) std::pow( as_int(), other.as_int() ), location() };

        case ValueType::FLOAT:
            return Value{ std::pow( as_int(), other.as_float() ), location() };

        default:
            ENGINE_PANIC( location(), "[", type(), "] ", dawn::op_pow, " [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ std::pow( as_float(), other.as_int() ), location() };

        case ValueType::FLOAT:
            return Value{ std::pow( as_float(), other.as_float() ), location() };

        default:
            ENGINE_PANIC( location(), "[", type(), "] ", dawn::op_pow, " [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        auto* op = left.get_method( __pow, false );
        if ( !op )
            ENGINE_PANIC( location(), "struct [", IDSystem::get( left.parent_id ), "] ", dawn::op_pow, " [", other.type(), "] not supported" );

        Value args[2] = { *this, other };
        return engine.handle_func( location(), *op, args, (Int) std::size( args ) );
    }

    default:
        ENGINE_PANIC( location(), "[", type(), "] ", dawn::op_pow, " [", other.type(), "] not supported" );
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
            return Value{ as_int() % other.as_int(), location() };

        case ValueType::FLOAT:
            return Value{ dawn_mod( (Float) as_int(), other.as_float() ), location() };

        default:
            ENGINE_PANIC( location(), "[", type(), "] ", dawn::op_mod, " [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ dawn_mod( as_float(), (Float) other.as_int() ), location() };

        case ValueType::FLOAT:
            return Value{ dawn_mod( as_float(), other.as_float() ), location() };

        default:
            ENGINE_PANIC( location(), "[", type(), "] ", dawn::op_mod, " [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        auto* op = left.get_method( __mod, false );
        if ( !op )
            ENGINE_PANIC( location(), "struct [", IDSystem::get( left.parent_id ), "] ", dawn::op_mod, " [", other.type(), "] not supported" );

        Value args[2] = { *this, other };
        return engine.handle_func( location(), *op, args, (Int) std::size( args ) );
    }

    default:
        ENGINE_PANIC( location(), "[", type(), "] ", dawn::op_mod, " [", other.type(), "] not supported" );
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
            return Value{ (Int) ( as_bool() <=> other.as_bool() )._Value, location() };

        default:
            ENGINE_PANIC( location(), "[", type(), "] ", dawn::op_cmpr, " [", other.type(), "] not supported" );
        }
    }

    case ValueType::INT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ (Int) ( as_int() <=> other.as_int() )._Value, location() };

        case ValueType::FLOAT:
            return Value{ (Int) ( as_int() <=> other.as_float() )._Value, location() };

        default:
            ENGINE_PANIC( location(), "[", type(), "] ", dawn::op_cmpr, " [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ (Int) ( as_float() <=> other.as_int() )._Value, location() };

        case ValueType::FLOAT:
            return Value{ (Int) ( as_float() <=> other.as_float() )._Value, location() };

        default:
            ENGINE_PANIC( location(), "[", type(), "] ", dawn::op_cmpr, " [", other.type(), "] not supported" );
        }
    }

    case ValueType::CHAR:
    {
        switch ( other.type() )
        {
        case ValueType::CHAR:
            return Value{ (Int) ( as_char() <=> other.as_char() )._Value, location() };

        default:
            ENGINE_PANIC( location(), "[", type(), "] ", dawn::op_cmpr, " [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRING:
    {
        switch ( other.type() )
        {
        case ValueType::STRING:
            return Value{ (Int) ( as_string() <=> other.as_string() )._Value, location() };

        default:
            ENGINE_PANIC( location(), "[", type(), "] ", dawn::op_cmpr, " [", other.type(), "] not supported" );
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
            if ( left.parent_id != right.parent_id )
                ENGINE_PANIC( location(), "enum [", IDSystem::get( left.parent_id ), "] ", dawn::op_cmpr, " enum [", IDSystem::get( right.parent_id ), "] not supported" );

            return Value{ (Int) ( left.key_id <=> right.key_id )._Value, location() };
        }

        default:
            ENGINE_PANIC( location(), "[", type(), "] ", dawn::op_cmpr, " [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        auto* op = left.get_method( __cmpr, false );
        if ( !op )
            ENGINE_PANIC( location(), "struct [", IDSystem::get( left.parent_id ), "] ", dawn::op_cmpr, " [", other.type(), "] not supported" );

        Value args[2] = { *this, other };
        return engine.handle_func( location(), *op, args, (Int) std::size( args ) );
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
                    return Value{ cmpr_res, location() };
            }
            return Value{ (Int) ( left.size() <=> right.size() )._Value, location() };
        }

        default:
            ENGINE_PANIC( location(), "[", type(), "] ", dawn::op_cmpr, " [", other.type(), "] not supported" );
        }
    }

    default:
        ENGINE_PANIC( location(), "[", type(), "] ", dawn::op_cmpr, " [", other.type(), "] not supported" );
    }
}

dawn::Value dawn::Value::op_eq( Engine& engine, Value const& other ) const
{
    auto result = op_cmpr( engine, other ).as_int();
    return Value{ result == 0, location() };
}

dawn::Value dawn::Value::op_neq( Engine& engine, Value const& other ) const
{
    auto result = op_cmpr( engine, other ).as_int();
    return Value{ result != 0, location() };
}

dawn::Value dawn::Value::op_less( Engine& engine, Value const& other ) const
{
    auto result = op_cmpr( engine, other ).as_int();
    return Value{ result < 0, location() };
}

dawn::Value dawn::Value::op_great( Engine& engine, Value const& other ) const
{
    auto result = op_cmpr( engine, other ).as_int();
    return Value{ result > 0, location() };
}

dawn::Value dawn::Value::op_lesseq( Engine& engine, Value const& other ) const
{
    auto result = op_cmpr( engine, other ).as_int();
    return Value{ result <= 0, location() };
}

dawn::Value dawn::Value::op_greateq( Engine& engine, Value const& other ) const
{
    auto result = op_cmpr( engine, other ).as_int();
    return Value{ result >= 0, location() };
}

dawn::Value dawn::Value::un_not() const
{
    return Value{ !as_bool(), location() };
}

dawn::Value dawn::Value::op_and( Value const& other ) const
{
    return Value{ as_bool() && other.as_bool(), location() };
}

dawn::Value dawn::Value::op_or( Value const& other ) const
{
    return Value{ as_bool() || other.as_bool(), location() };
}

dawn::Value dawn::Value::op_range( Engine& engine, Value const& other ) const
{
    RangeValue result;
    result.start_incl = as_int();
    result.end_excl = other.as_int();
    return Value{ result, location() };
}

dawn::Bool dawn::Value::to_bool( Engine& engine ) const
{
    switch ( type() )
    {
    case ValueType::VOID:
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
            ENGINE_PANIC( location(), "can not convert struct [", IDSystem::get( left.parent_id ), "] to bool" );

        Value args[1] = { *this };
        return engine.handle_func( location(), *method, args, (Int) std::size( args ) ).as_bool();
    }

    default:
        ENGINE_PANIC( location(), "can not convert [", type(), "] to bool" );
    }
}

dawn::Int dawn::Value::to_int( Engine& engine ) const
{
    switch ( type() )
    {
    case ValueType::VOID:
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
        throw Value{ dawn::format( "string \"", as_string(), "\" to int failed" ), location() };
    }

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        auto* method = left.get_method( _int, true );
        if ( !method )
            ENGINE_PANIC( location(), "can not convert struct [", IDSystem::get( left.parent_id ), "] to int" );

        Value args[1] = { *this };
        return engine.handle_func( location(), *method, args, (Int) std::size( args ) ).as_int();
    }

    default:
        ENGINE_PANIC( location(), "can not convert [", type(), "] to int" );
    }
}

dawn::Float dawn::Value::to_float( Engine& engine ) const
{
    switch ( type() )
    {
    case ValueType::VOID:
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
        throw Value{ dawn::format( "string \"", as_string(), "\" to float failed" ), location() };
    }

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        auto* method = left.get_method( _float, true );
        if ( !method )
            ENGINE_PANIC( location(), "can not convert struct [", IDSystem::get( left.parent_id ), "] to float" );

        Value args[1] = { *this };
        return engine.handle_func( location(), *method, args, (Int) std::size( args ) ).as_float();
    }

    default:
        ENGINE_PANIC( location(), "can not convert [", type(), "] to float" );
    }
}

dawn::Char dawn::Value::to_char( Engine& engine ) const
{
    switch ( type() )
    {
    case ValueType::VOID:
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
            ENGINE_PANIC( location(), "can not convert struct [", IDSystem::get( left.parent_id ), "] to char" );

        Value args[1] = { *this };
        return engine.handle_func( location(), *method, args, (Int) std::size( args ) ).as_char();
    }

    default:
        ENGINE_PANIC( location(), "can not convert [", type(), "] to char" );
    }
}

dawn::String dawn::Value::to_string( Engine& engine ) const
{
    switch ( type() )
    {
    case ValueType::VOID:
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
            stream << IDSystem::get( method.self->as_struct().parent_id )
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
                    stream << dfunc->args[i].type << ' ' << IDSystem::get( dfunc->args[i].id ) << op_split << ' ';
                stream << dfunc->args.back().type << ' ' << IDSystem::get( dfunc->args.back().id );
            }
        }

        stream << ( func.is_lambda() ? op_lambda : op_expr_cls );
        return stream.str();
    }

    case ValueType::ENUM:
    {
        auto const& value = as_enum();
        return format( IDSystem::get( value.parent_id ), op_link, IDSystem::get( value.key_id ) );
    }

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        if ( auto* method = left.get_method( _string, true ) )
        {
            Value args[1] = { *this };
            return engine.handle_func( location(), *method, args, (Int) std::size( args ) ).as_string();
        }
        else
        {
            StringStream stream;
            stream << IDSystem::get( left.parent_id );
            if ( left.fields.empty() )
            {
                stream << op_scope_opn << op_scope_cls;
                return stream.str();
            }

            auto it = left.fields.begin();
            stream << op_scope_opn;
            for ( ; it != --left.fields.end(); ++it )
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
        ENGINE_PANIC( location(), "can not convert [", type(), "] to string" );
    }
}

dawn::FunctionValue dawn::Value::to_function( Engine& engine ) const
{
    switch ( type() )
    {
    case ValueType::FUNCTION:
        return as_function();

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        auto* method = left.get_method( _function, true );
        if ( !method )
            ENGINE_PANIC( location(), "can not convert struct [", IDSystem::get( left.parent_id ), "] to function" );

        Value args[1] = { *this };
        return engine.handle_func( location(), *method, args, (Int) std::size( args ) ).as_function();
    }

    default:
        ENGINE_PANIC( location(), "can not convert [", type(), "] to function" );
    }
}

dawn::ArrayValue dawn::Value::to_array( Engine& engine ) const
{
    switch ( type() )
    {
    case ValueType::VOID:
        return ArrayValue{};

    case ValueType::STRING:
    {
        auto& strval = as_string();
        ArrayValue result;
        result.data.reserve( strval.size() );
        for ( Char c : strval )
            result.data.emplace_back( c, location() );
        return result;
    }

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        auto* method = left.get_method( _array, true );
        if ( !method )
            ENGINE_PANIC( location(), "can not convert struct [", IDSystem::get( left.parent_id ), "] to array" );

        Value args[1] = { *this };
        return engine.handle_func( location(), *method, args, (Int) std::size( args ) ).as_array();
    }

    case ValueType::ARRAY:
        return as_array();

    default:
        ENGINE_PANIC( location(), "can not convert [", type(), "] to array" );
    }
}

dawn::RangeValue dawn::Value::to_range( Engine& engine ) const
{
    switch ( type() )
    {
    case ValueType::VOID:
        return RangeValue{};

    case ValueType::INT:
        return RangeValue{ 0, as_int() };

    case ValueType::STRUCT:
    {
        auto& left = as_struct();
        auto* method = left.get_method( _range, true );
        if ( !method )
            ENGINE_PANIC( location(), "can not convert struct [", IDSystem::get( left.parent_id ), "] to range" );

        Value args[1] = { *this };
        return engine.handle_func( location(), *method, args, (Int) std::size( args ) ).as_range();
    }

    case ValueType::RANGE:
        return as_range();

    default:
        ENGINE_PANIC( location(), "can not convert [", type(), "] to range" );
    }
}
