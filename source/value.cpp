#include "value.h"
#include "pool.h"
#include "engine.h"


#define ID_HELPER(id) static const ID _##id = IDSystem::get( #id )

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
    members.reserve( other.members.size() );
    for ( auto& [key, member] : other.members )
        members[key] = { .value = member.value.clone(), .type = member.type };
}

dawn::StructValue& dawn::StructValue::operator=( StructValue const& other )
{
    if ( this != &other )
    {
        parent_id = other.parent_id;
        members.clear();
        members.reserve( other.members.size() );
        for ( auto& [key, member] : other.members )
            members[key] = { .value = member.value.clone(), .type = member.type };
    }
    return *this;
}

dawn::StructValue::StructValue( StructValue&& other ) noexcept
    : parent_id( std::move( other.parent_id ) )
    , members( std::move( other.members ) )
{
}

dawn::StructValue& dawn::StructValue::operator=( StructValue&& other ) noexcept
{
    if ( this != &other )
    {
        parent_id = std::move( other.parent_id );
        members = std::move( other.members );
    }
    return *this;
}

dawn::FunctionValue* dawn::StructValue::get_method( ID id )
{
    const auto it = members.find( id );
    if ( it == members.end() )
        return nullptr;
    if ( it->second.type != MemberType::METHOD )
        return nullptr;
    return &it->second.value.as_function();
}

dawn::FunctionValue* dawn::StructValue::get_unary( ID id )
{
    const auto it = members.find( id );
    if ( it == members.end() )
        return nullptr;
    if ( it->second.type != MemberType::METHOD )
        return nullptr;
    auto& func = it->second.value.as_function();
    if ( auto* dfunc = func.dfunction() )
    {
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
        for ( auto& [_, member] : result.as_struct().members )
        {
            if ( member.type == MemberType::METHOD )
                *member.value.as_function().as_method().self = result;
        }
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
        for ( auto& [_, member] : value.members )
        {
            if ( member.type == MemberType::FIELD )
                member.value.unlock_const();
        }
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
        auto* method = left.get_method( _bool );
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
        auto* method = left.get_method( _int );
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
        auto* method = left.get_method( _float );
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
        auto* method = left.get_method( _char );
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
        if ( auto* method = left.get_method( _string ) )
        {
            Value args[1] = { *this };
            return engine.handle_func( location(), *method, args, (Int) std::size( args ) ).as_string();
        }
        else
        {
            StringStream stream;
            stream << IDSystem::get( left.parent_id );
            if ( left.members.empty() )
            {
                stream << op_scope_opn << op_scope_cls;
                return stream.str();
            }

            auto it = left.members.begin();
            stream << op_scope_opn;
            for ( ; it != --left.members.end(); ++it )
                stream << IDSystem::get( it->first ) << op_assign << it->second.value.to_string( engine ) << op_split << ' ';
            stream << IDSystem::get( it->first ) << op_assign << it->second.value.to_string( engine ) << op_scope_cls;
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
        auto* method = left.get_method( _function );
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
        auto* method = left.get_method( _array );
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
        auto* method = left.get_method( _range );
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
