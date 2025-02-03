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

dawn::StructVal::StructVal( StructVal const& other )
{
    parent = other.parent;
    for ( auto& [key, val] : other.members )
        members[key] = val.clone();
}

dawn::StructVal& dawn::StructVal::operator=( StructVal const& other )
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

dawn::Value* dawn::StructVal::get_member( Int id )
{
    auto it = members.find( id );
    if ( it == members.end() )
        return nullptr;
    return &it->second;
}

dawn::Function* dawn::StructVal::get_method( Int id, Bool unary )
{
    auto it = members.find( id );
    if ( it == members.end() )
        return nullptr;

    if ( it->second.type() != ValueType::FUNCTION )
        return nullptr;
    auto& func = it->second.as<Function>();

    if ( unary && func.args.size() != 1 )
        return nullptr;
    return &func;
}

dawn::ArrayVal::ArrayVal( ArrayVal const& other )
{
    data.reserve( other.data.size() );
    for ( auto& val : other.data )
        data.push_back( val.clone() );
}

dawn::ArrayVal& dawn::ArrayVal::operator=( ArrayVal const& other )
{
    if ( this != &other )
    {
        data.clear();
        data.reserve( other.data.size() );
        for ( auto& val : other.data )
            data.push_back( val.clone() );
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

dawn::Value::Value( EnumVal const& value )
    : m_regref( enum_pool().new_register().cast<Void>() ), m_type( ValueType::ENUM )
{
    m_regref.cast<EnumVal>().value() = value;
}

dawn::Value::Value( StructVal const& value )
    : m_regref( struct_pool().new_register().cast<Void>() ), m_type( ValueType::STRUCT )
{
    m_regref.cast<StructVal>().value() = value;
}

dawn::Value::Value( ArrayVal const& value )
    : m_regref( array_pool().new_register().cast<Void>() ), m_type( ValueType::ARRAY )
{
    m_regref.cast<ArrayVal>().value() = value;
}

dawn::Value::Value( RangeVal const& value )
    : m_regref( range_pool().new_register().cast<Void>() ), m_type( ValueType::RANGE )
{
    m_regref.cast<RangeVal>().value() = value;
}

dawn::ValueType dawn::Value::type() const
{
    return m_type;
}

void dawn::Value::assign( Value const& other )
{
    if ( m_type != other.m_type )
        PANIC( "can't assign [", other.m_type, "] to [", m_type, "]" );

    switch ( m_type )
    {
    case ValueType::BOOL:
        as<Bool>() = other.as<Bool>();
        break;

    case ValueType::INT:
        as<Int>() = other.as<Int>();
        break;

    case ValueType::FLOAT:
        as<Float>() = other.as<Float>();
        break;

    case ValueType::CHAR:
        as<Char>() = other.as<Char>();
        break;

    case ValueType::STRING:
        as<String>() = other.as<String>();
        break;

    case ValueType::FUNCTION:
        as<Function>() = other.as<Function>();
        break;

    case ValueType::ENUM:
        as<EnumVal>() = other.as<EnumVal>();
        break;

    case ValueType::STRUCT:
        as<StructVal>() = other.as<StructVal>();
        break;

    case ValueType::ARRAY:
        as<ArrayVal>() = other.as<ArrayVal>();
        break;

    case ValueType::RANGE:
        as<RangeVal>() = other.as<RangeVal>();
        break;

    default:
        PANIC( "assign to [", m_type, "] not supported" );
    }
}

dawn::Value dawn::Value::clone() const
{
    switch ( m_type )
    {
    case ValueType::NOTHING:
        return Value{};

    case ValueType::BOOL:
        return Value{ as<Bool>() };

    case ValueType::INT:
        return Value{ as<Int>() };

    case ValueType::FLOAT:
        return Value{ as<Float>() };

    case ValueType::CHAR:
        return Value{ as<Char>() };

    case ValueType::STRING:
        return Value{ as<String>() };

    case ValueType::FUNCTION:
        return Value{ as<Function>() };

    case ValueType::ENUM:
        return Value{ as<EnumVal>() };

    case ValueType::STRUCT:
        return Value{ as<StructVal>() };

    case ValueType::ARRAY:
        return Value{ as<ArrayVal>() };

    case ValueType::RANGE:
        return Value{ as<RangeVal>() };

    default:
        PANIC( "Can't clone [", Int( m_type ), "]" );
    }
}

dawn::Value dawn::Value::un_plus( Engine& engine ) const
{
    switch ( type() )
    {
    case ValueType::INT:
        return Value{ +as<Int>() };

    case ValueType::FLOAT:
        return Value{ +as<Float>() };

    case ValueType::STRUCT:
    {
        auto& left = as<StructVal>();
        Function* op = left.get_method( __add, true );
        if ( !op )
            PANIC( "+ struct [", IDSystem::get( left.parent->id ), "] not supported" );

        return engine.handle_func( *op, this, 1 );
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
        return Value{ -as<Int>() };

    case ValueType::FLOAT:
        return Value{ -as<Float>() };

    case ValueType::STRUCT:
    {
        auto& left = as<StructVal>();
        Function* op = left.get_method( __sub, true );
        if ( !op )
            PANIC( "- struct [", IDSystem::get( left.parent->id ), "] not supported" );

        return engine.handle_func( *op, this, 1 );
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
            return Value{ as<Int>() + other.as<Int>() };

        case ValueType::FLOAT:
            return Value{ as<Int>() + other.as<Float>() };

        default:
            PANIC( "[", type(), "] + [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ as<Float>() + other.as<Int>() };

        case ValueType::FLOAT:
            return Value{ as<Float>() + other.as<Float>() };

        default:
            PANIC( "[", type(), "] + [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRING:
    {
        switch ( other.type() )
        {
        case ValueType::STRING:
            return Value{ as<String>() + other.as<String>() };

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
            ArrayVal result;
            result.data.insert( result.data.end(), as<ArrayVal>().data.begin(), as<ArrayVal>().data.end() );
            result.data.insert( result.data.end(), other.as<ArrayVal>().data.begin(), other.as<ArrayVal>().data.end() );
            return Value{ result };
        }

        default:
            PANIC( "[", type(), "] + [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as<StructVal>();
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
            return Value{ as<Int>() - other.as<Int>() };

        case ValueType::FLOAT:
            return Value{ as<Int>() - other.as<Float>() };

        default:
            PANIC( "[", type(), "] - [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ as<Float>() - other.as<Int>() };

        case ValueType::FLOAT:
            return Value{ as<Float>() - other.as<Float>() };

        default:
            PANIC( "[", type(), "] - [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as<StructVal>();
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
            return Value{ as<Int>() * other.as<Int>() };

        case ValueType::FLOAT:
            return Value{ as<Int>() * other.as<Float>() };

        default:
            PANIC( "[", type(), "] * [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ as<Float>() * other.as<Int>() };

        case ValueType::FLOAT:
            return Value{ as<Float>() * other.as<Float>() };

        default:
            PANIC( "[", type(), "] * [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as<StructVal>();
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
            return Value{ as<Int>() / other.as<Int>() };

        case ValueType::FLOAT:
            return Value{ as<Int>() / other.as<Float>() };

        default:
            PANIC( "[", type(), "] / [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ as<Float>() / other.as<Int>() };

        case ValueType::FLOAT:
            return Value{ as<Float>() / other.as<Float>() };

        default:
            PANIC( "[", type(), "] / [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as<StructVal>();
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
            return Value{ (Int) std::pow( as<Int>(), other.as<Int>() ) };

        case ValueType::FLOAT:
            return Value{ std::pow( as<Int>(), other.as<Float>() ) };

        default:
            PANIC( "[", type(), "] ^ [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ std::pow( as<Float>(), other.as<Int>() ) };

        case ValueType::FLOAT:
            return Value{ std::pow( as<Float>(), other.as<Float>() ) };

        default:
            PANIC( "[", type(), "] ^ [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as<StructVal>();
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
            return Value{ as<Int>() % other.as<Int>() };

        case ValueType::FLOAT:
            return Value{ mymod( (Float) as<Int>(), other.as<Float>() ) };

        default:
            PANIC( "[", type(), "] % [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ mymod( as<Float>(), (Float) other.as<Int>() ) };

        case ValueType::FLOAT:
            return Value{ mymod( as<Float>(), other.as<Float>() ) };

        default:
            PANIC( "[", type(), "] % [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as<StructVal>();
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
            return Value{ (Int) (as<Bool>() <=> other.as<Bool>())._Value };

        default:
            PANIC( "[", type(), "] <=> [", other.type(), "] not supported" );
        }
    }

    case ValueType::INT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ (Int) (as<Int>() <=> other.as<Int>())._Value };

        case ValueType::FLOAT:
            return Value{ (Int) (as<Int>() <=> other.as<Float>())._Value };

        default:
            PANIC( "[", type(), "] <=> [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ (Int) (as<Float>() <=> other.as<Int>())._Value };

        case ValueType::FLOAT:
            return Value{ (Int) (as<Float>() <=> other.as<Float>())._Value };

        default:
            PANIC( "[", type(), "] <=> [", other.type(), "] not supported" );
        }
    }

    case ValueType::CHAR:
    {
        switch ( other.type() )
        {
        case ValueType::CHAR:
            return Value{ (Int) (as<Char>() <=> other.as<Char>())._Value };

        default:
            PANIC( "[", type(), "] <=> [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRING:
    {
        switch ( other.type() )
        {
        case ValueType::STRING:
            return Value{ (Int) (as<String>() <=> other.as<String>())._Value };

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
            auto& left = as<EnumVal>();
            auto& right = other.as<EnumVal>();
            if ( left.parent != right.parent )
                PANIC( "enum [", IDSystem::get( left.parent->id ), "] <=> enum [", IDSystem::get( right.parent->id ), "] not supported" );

            return Value{ (Int) (left.key_id <=> right.key_id)._Value };
        }

        default:
            PANIC( "[", type(), "] <=> [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as<StructVal>();
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
            auto& left = as<ArrayVal>().data;
            auto& right = other.as<ArrayVal>().data;

            for ( Int i = 0; i < (Int) std::min( left.size(), right.size() ); i++ )
            {
                Int cmpr_res = left[i].op_cmpr( engine, right[i] ).as<Int>();
                if ( cmpr_res != 0 )
                    return Value{ cmpr_res };
            }
            return Value{ (Int) (left.size() <=> right.size())._Value };
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
    auto result = op_cmpr( engine, other ).as<Int>();
    return Value{ result == 0 };
}

dawn::Value dawn::Value::op_neq( Engine& engine, Value const& other ) const
{
    auto result = op_cmpr( engine, other ).as<Int>();
    return Value{ result != 0 };
}

dawn::Value dawn::Value::op_less( Engine& engine, Value const& other ) const
{
    auto result = op_cmpr( engine, other ).as<Int>();
    return Value{ result < 0 };
}

dawn::Value dawn::Value::op_great( Engine& engine, Value const& other ) const
{
    auto result = op_cmpr( engine, other ).as<Int>();
    return Value{ result > 0 };
}

dawn::Value dawn::Value::op_lesseq( Engine& engine, Value const& other ) const
{
    auto result = op_cmpr( engine, other ).as<Int>();
    return Value{ result <= 0 };
}

dawn::Value dawn::Value::op_greateq( Engine& engine, Value const& other ) const
{
    auto result = op_cmpr( engine, other ).as<Int>();
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
    RangeVal result;
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
        return as<Bool>();

    case ValueType::INT:
        return (Bool) as<Int>();

    case ValueType::FLOAT:
        return (Bool) as<Float>();

    case ValueType::CHAR:
        return (Bool) as<Char>();

    case ValueType::STRING:
        return as<String>() == kw_true;

    case ValueType::STRUCT:
    {
        auto& left = as<StructVal>();
        Function* method = left.get_method( _to_bool, true );
        if ( !method )
            PANIC( "can't convert struct [", IDSystem::get( left.parent->id ), "] to bool" );

        return engine.handle_func( *method, this, 1 ).to_bool( engine );
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
        return (Int) as<Bool>();

    case ValueType::INT:
        return as<Int>();

    case ValueType::FLOAT:
        return (Int) as<Float>();

    case ValueType::CHAR:
        return (Int) as<Char>();

    case ValueType::STRING:
    {
        if ( auto optres = parse_int( as<String>() ) )
            return *optres;
        throw Value{ dawn::format( "string \"", as<String>(), "\" to int failed" ) };
    }

    case ValueType::STRUCT:
    {
        auto& left = as<StructVal>();
        Function* method = left.get_method( _to_int, true );
        if ( !method )
            PANIC( "can't convert struct [", IDSystem::get( left.parent->id ), "] to int" );

        return engine.handle_func( *method, this, 1 ).to_int( engine );
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
        return (Float) as<Bool>();

    case ValueType::INT:
        return (Float) as<Int>();

    case ValueType::FLOAT:
        return as<Float>();

    case ValueType::CHAR:
        return (Float) as<Char>();

    case ValueType::STRING:
    {
        if ( auto optres = parse_float( as<String>() ) )
            return *optres;
        throw Value{ dawn::format( "string \"", as<String>(), "\" to float failed" ) };
    }

    case ValueType::STRUCT:
    {
        auto& left = as<StructVal>();
        Function* method = left.get_method( _to_float, true );
        if ( !method )
            PANIC( "can't convert struct [", IDSystem::get( left.parent->id ), "] to float" );

        return engine.handle_func( *method, this, 1 ).to_float( engine );
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
        return as<Bool>() ? kw_true[0] : kw_false[0];

    case ValueType::INT:
        return (Char) as<Int>();

    case ValueType::FLOAT:
        return (Char) as<Float>();

    case ValueType::CHAR:
        return as<Char>();

    case ValueType::STRING:
        return as<String>()[0];

    case ValueType::STRUCT:
    {
        auto& left = as<StructVal>();
        Function* method = left.get_method( _to_char, true );
        if ( !method )
            PANIC( "can't convert struct [", IDSystem::get( left.parent->id ), "] to char" );

        return engine.handle_func( *method, this, 1 ).to_char( engine );
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
        return String{ as<Bool>() ? kw_true : kw_false };

    case ValueType::INT:
        return std::to_string( as<Int>() );

    case ValueType::FLOAT:
    {
        String result = format( as<Float>() );
        if ( std::to_string( (Int) as<Float>() ) == result )
            result += ".0";
        return result;
    }

    case ValueType::CHAR:
        return String( 1, as<Char>() );

    case ValueType::STRING:
        return as<String>();

    case ValueType::FUNCTION:
    {
        auto& func = as<Function>();
        if ( func.is_lambda() )
            return "lambda()";
        else if ( func.is_method() )
            return format( IDSystem::get( func.self->as<StructVal>().parent->id ), "::", IDSystem::get( func.id ), "()" );
        else
            return format( IDSystem::get( func.id ), "()" );
    }

    case ValueType::ENUM:
        return IDSystem::get( as<EnumVal>().key_id );

    case ValueType::STRUCT:
    {
        auto& left = as<StructVal>();
        Function* method = left.get_method( _to_string, true );
        if ( !method )
            return format( IDSystem::get( left.parent->id ), "{}" );

        return engine.handle_func( *method, this, 1 ).to_string( engine );
    }

    case ValueType::ARRAY:
    {
        auto& val = as<ArrayVal>();
        if ( val.data.empty() )
            return "[]";

        StringStream stream;
        stream << "[";
        for ( Int i = 0; i < (Int) val.data.size() - 1; i++ )
            stream << val.data[i].to_string( engine ) << ", ";
        stream << val.data.back().to_string( engine ) << "]";
        return stream.str();
    }

    case ValueType::RANGE:
    {
        auto& val = as<RangeVal>();
        return format( '[', val.start_incl, ", ", val.end_excl, ')' );
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
