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
        members[key] = ValueRef{ val.value(), val.kind() };
}

dawn::StructVal& dawn::StructVal::operator=( StructVal const& other )
{
    if ( this != &other )
    {
        parent = other.parent;
        members.clear();
        for ( auto& [key, val] : other.members )
            members[key] = ValueRef{ val.value(), val.kind() };
    }
    return *this;
}

dawn::ValueRef* dawn::StructVal::get_member( Int id )
{
    auto it = members.find( id );
    if ( it == members.end() )
        return nullptr;
    return &it->second;
}

dawn::Function* dawn::StructVal::get_method( Int id, Bool is_unary )
{
    auto it = members.find( id );
    if ( it == members.end() )
        return nullptr;
    if ( it->second.type() != ValueType::FUNCTION )
        return nullptr;

    auto& func = it->second.as<Function>();
    if ( is_unary && !func.is_unary_op() )
        return nullptr;
    return &func;
}

dawn::ArrayVal::ArrayVal( ArrayVal const& other )
{
    data.reserve( other.data.size() );
    for ( auto& val : other.data )
        data.emplace_back( val.value(), val.kind() );
}

dawn::ArrayVal& dawn::ArrayVal::operator=( ArrayVal const& other )
{
    if ( this != &other )
    {
        data.clear();
        data.reserve( other.data.size() );
        for ( auto& val : other.data )
            data.emplace_back( val.value(), val.kind() );
    }
    return *this;
}

dawn::ValueRef::ValueRef( Bool value, ValueKind kind )
    : m_regref( value_pool().new_register() ), m_kind( kind )
{
    m_regref.value().emplace<Bool>( value );
}

dawn::ValueRef::ValueRef( Int value, ValueKind kind )
    : m_regref( value_pool().new_register() ), m_kind( kind )
{
    m_regref.value().emplace<Int>( value );
}

dawn::ValueRef::ValueRef( Float value, ValueKind kind )
    : m_regref( value_pool().new_register() ), m_kind( kind )
{
    m_regref.value().emplace<Float>( value );
}

dawn::ValueRef::ValueRef( Char value, ValueKind kind )
    : m_regref( value_pool().new_register() ), m_kind( kind )
{
    m_regref.value().emplace<Char>( value );
}

dawn::ValueRef::ValueRef( StringRef const& value, ValueKind kind )
    : m_regref( value_pool().new_register() ), m_kind( kind )
{
    m_regref.value().emplace<String>( value );
}

dawn::ValueRef::ValueRef( Function const& value, ValueKind kind )
    : m_regref( value_pool().new_register() ), m_kind( kind )
{
    m_regref.value().emplace<Function>( value );
}

dawn::ValueRef::ValueRef( EnumVal const& value, ValueKind kind )
    : m_regref( value_pool().new_register() ), m_kind( kind )
{
    m_regref.value().emplace<EnumVal>( value );
}

dawn::ValueRef::ValueRef( StructVal const& value, ValueKind kind )
    : m_regref( value_pool().new_register() ), m_kind( kind )
{
    m_regref.value().emplace<StructVal>( value );
    reapply_kind();
}

dawn::ValueRef::ValueRef( ArrayVal const& value, ValueKind kind )
    : m_regref( value_pool().new_register() ), m_kind( kind )
{
    m_regref.value().emplace<ArrayVal>( value );
    reapply_kind();
}

dawn::ValueRef::ValueRef( RangeVal const& value, ValueKind kind )
    : m_regref( value_pool().new_register() ), m_kind( kind )
{
    m_regref.value().emplace<RangeVal>( value );
}

dawn::ValueRef::ValueRef( Value const& value, ValueKind kind )
    : m_regref( value_pool().new_register() ), m_kind( kind )
{
    m_regref.value() = value;
    reapply_kind();
}

dawn::ValueKind dawn::ValueRef::kind() const
{
    return m_kind;
}

dawn::ValueType dawn::ValueRef::type() const
{
    return m_regref.value().type();
}

dawn::Value const& dawn::ValueRef::value() const
{
    return m_regref.value();
}

void dawn::ValueRef::set_value( Value const& value )
{
    if ( m_kind == ValueKind::LET )
        PANIC( "can't set value of a let variable" );

    m_regref.value() = value;
    reapply_kind();
}

dawn::Bool dawn::ValueRef::valid() const noexcept
{
    return m_regref.valid();
}

dawn::ValueRef::operator bool() const noexcept
{
    return m_regref.valid();
}

dawn::ValueRef dawn::ValueRef::un_plus( Engine& engine ) const
{
    switch ( type() )
    {
    case ValueType::INT:
        return ValueRef{ +as<Int>() };

    case ValueType::FLOAT:
        return ValueRef{ +as<Float>() };

    case ValueType::STRUCT:
    {
        auto& left = as<StructVal>();
        Function* op = left.get_method( __add, true );
        if ( !op )
            PANIC( "+ struct [", IDSystem::get( left.parent->id ), "] not supported" );

        op->self_vals.resize( 1 );
        op->self_vals[0] = *this;

        return engine.handle_func( *op, op->self_vals.data(), (Int) op->self_vals.size() );
    }

    default:
        PANIC( "+ [", type(), "] not supported" );
    }
}

dawn::ValueRef dawn::ValueRef::un_minus( Engine& engine ) const
{
    switch ( type() )
    {
    case ValueType::INT:
        return ValueRef{ -as<Int>() };

    case ValueType::FLOAT:
        return ValueRef{ -as<Float>() };

    case ValueType::STRUCT:
    {
        auto& left = as<StructVal>();
        Function* op = left.get_method( __sub, true );
        if ( !op )
            PANIC( "- struct [", IDSystem::get( left.parent->id ), "] not supported" );

        op->self_vals.resize( 1 );
        op->self_vals[0] = *this;

        return engine.handle_func( *op, op->self_vals.data(), (Int) op->self_vals.size() );
    }

    default:
        PANIC( "- [", type(), "] not supported" );
    }
}

dawn::ValueRef dawn::ValueRef::op_add( Engine& engine, ValueRef const& other ) const
{
    switch ( type() )
    {
    case ValueType::INT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return ValueRef{ as<Int>() + other.as<Int>() };

        case ValueType::FLOAT:
            return ValueRef{ as<Int>() + other.as<Float>() };

        default:
            PANIC( "[", type(), "] + [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return ValueRef{ as<Float>() + other.as<Int>() };

        case ValueType::FLOAT:
            return ValueRef{ as<Float>() + other.as<Float>() };

        default:
            PANIC( "[", type(), "] + [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRING:
    {
        switch ( other.type() )
        {
        case ValueType::STRING:
            return ValueRef{ as<String>() + other.as<String>() };

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
            return ValueRef{ result };
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
            PANIC( "struct [", IDSystem::get( left.parent->id ), "] + struct [", other.type(), "] not supported" );

        op->self_vals.resize( 2 );
        op->self_vals[0] = *this;
        op->self_vals[1] = other;

        return engine.handle_func( *op, op->self_vals.data(), (Int) op->self_vals.size() );
    }

    default:
        PANIC( "[", type(), "] + [", other.type(), "] not supported" );
    }
}

dawn::ValueRef dawn::ValueRef::op_sub( Engine& engine, ValueRef const& other ) const
{
    switch ( type() )
    {
    case ValueType::INT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return ValueRef{ as<Int>() - other.as<Int>() };

        case ValueType::FLOAT:
            return ValueRef{ as<Int>() - other.as<Float>() };

        default:
            PANIC( "[", type(), "] - [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return ValueRef{ as<Float>() - other.as<Int>() };

        case ValueType::FLOAT:
            return ValueRef{ as<Float>() - other.as<Float>() };

        default:
            PANIC( "[", type(), "] - [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as<StructVal>();
        Function* op = left.get_method( __sub, false );
        if ( !op )
            PANIC( "struct [", IDSystem::get( left.parent->id ), "] - struct [", other.type(), "] not supported" );

        op->self_vals.resize( 2 );
        op->self_vals[0] = *this;
        op->self_vals[1] = other;

        return engine.handle_func( *op, op->self_vals.data(), (Int) op->self_vals.size() );
    }

    default:
        PANIC( "[", type(), "] - [", other.type(), "] not supported" );
    }
}

dawn::ValueRef dawn::ValueRef::op_mul( Engine& engine, ValueRef const& other ) const
{
    switch ( type() )
    {
    case ValueType::INT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return ValueRef{ as<Int>() * other.as<Int>() };

        case ValueType::FLOAT:
            return ValueRef{ as<Int>() * other.as<Float>() };

        default:
            PANIC( "[", type(), "] * [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return ValueRef{ as<Float>() * other.as<Int>() };

        case ValueType::FLOAT:
            return ValueRef{ as<Float>() * other.as<Float>() };

        default:
            PANIC( "[", type(), "] * [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as<StructVal>();
        Function* op = left.get_method( __mul, false );
        if ( !op )
            PANIC( "struct [", IDSystem::get( left.parent->id ), "] * struct [", other.type(), "] not supported" );

        op->self_vals.resize( 2 );
        op->self_vals[0] = *this;
        op->self_vals[1] = other;

        return engine.handle_func( *op, op->self_vals.data(), (Int) op->self_vals.size() );
    }

    default:
        PANIC( "[", type(), "] * [", other.type(), "] not supported" );
    }
}

dawn::ValueRef dawn::ValueRef::op_div( Engine& engine, ValueRef const& other ) const
{
    switch ( type() )
    {
    case ValueType::INT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return ValueRef{ as<Int>() / other.as<Int>() };

        case ValueType::FLOAT:
            return ValueRef{ as<Int>() / other.as<Float>() };

        default:
            PANIC( "[", type(), "] / [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return ValueRef{ as<Float>() / other.as<Int>() };

        case ValueType::FLOAT:
            return ValueRef{ as<Float>() / other.as<Float>() };

        default:
            PANIC( "[", type(), "] / [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as<StructVal>();
        Function* op = left.get_method( __div, false );
        if ( !op )
            PANIC( "struct [", IDSystem::get( left.parent->id ), "] / struct [", other.type(), "] not supported" );

        op->self_vals.resize( 2 );
        op->self_vals[0] = *this;
        op->self_vals[1] = other;

        return engine.handle_func( *op, op->self_vals.data(), (Int) op->self_vals.size() );
    }

    default:
        PANIC( "[", type(), "] / [", other.type(), "] not supported" );
    }
}

dawn::ValueRef dawn::ValueRef::op_pow( Engine& engine, ValueRef const& other ) const
{
    switch ( type() )
    {
    case ValueType::INT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return ValueRef{ (Int) std::pow( as<Int>(), other.as<Int>() ) };

        case ValueType::FLOAT:
            return ValueRef{ std::pow( as<Int>(), other.as<Float>() ) };

        default:
            PANIC( "[", type(), "] ^ [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return ValueRef{ std::pow( as<Float>(), other.as<Int>() ) };

        case ValueType::FLOAT:
            return ValueRef{ std::pow( as<Float>(), other.as<Float>() ) };

        default:
            PANIC( "[", type(), "] ^ [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as<StructVal>();
        Function* op = left.get_method( __pow, false );
        if ( !op )
            PANIC( "struct [", IDSystem::get( left.parent->id ), "] ^ struct [", other.type(), "] not supported" );

        op->self_vals.resize( 2 );
        op->self_vals[0] = *this;
        op->self_vals[1] = other;

        return engine.handle_func( *op, op->self_vals.data(), (Int) op->self_vals.size() );
    }

    default:
        PANIC( "[", type(), "] ^ [", other.type(), "] not supported" );
    }
}

dawn::ValueRef dawn::ValueRef::op_mod( Engine& engine, ValueRef const& other ) const
{
    switch ( type() )
    {
    case ValueType::INT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return ValueRef{ as<Int>() % other.as<Int>() };

        case ValueType::FLOAT:
            return ValueRef{ mymod( (Float) as<Int>(), other.as<Float>() ) };

        default:
            PANIC( "[", type(), "] % [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return ValueRef{ mymod( as<Float>(), (Float) other.as<Int>() ) };

        case ValueType::FLOAT:
            return ValueRef{ mymod( as<Float>(), other.as<Float>() ) };

        default:
            PANIC( "[", type(), "] % [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as<StructVal>();
        Function* op = left.get_method( __mod, false );
        if ( !op )
            PANIC( "struct [", IDSystem::get( left.parent->id ), "] % struct [", other.type(), "] not supported" );

        op->self_vals.resize( 2 );
        op->self_vals[0] = *this;
        op->self_vals[1] = other;

        return engine.handle_func( *op, op->self_vals.data(), (Int) op->self_vals.size() );
    }

    default:
        PANIC( "[", type(), "] % [", other.type(), "] not supported" );
    }
}

dawn::ValueRef dawn::ValueRef::op_cmpr( Engine& engine, ValueRef const& other ) const
{
    switch ( type() )
    {
    case ValueType::BOOL:
    {
        switch ( other.type() )
        {
        case ValueType::BOOL:
            return ValueRef{ (Int) (as<Bool>() <=> other.as<Bool>())._Value };

        default:
            PANIC( "[", type(), "] <=> [", other.type(), "] not supported" );
        }
    }

    case ValueType::INT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return ValueRef{ (Int) (as<Int>() <=> other.as<Int>())._Value };

        case ValueType::FLOAT:
            return ValueRef{ (Int) (as<Int>() <=> other.as<Float>())._Value };

        default:
            PANIC( "[", type(), "] <=> [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return ValueRef{ (Int) (as<Float>() <=> other.as<Int>())._Value };

        case ValueType::FLOAT:
            return ValueRef{ (Int) (as<Float>() <=> other.as<Float>())._Value };

        default:
            PANIC( "[", type(), "] <=> [", other.type(), "] not supported" );
        }
    }

    case ValueType::CHAR:
    {
        switch ( other.type() )
        {
        case ValueType::CHAR:
            return ValueRef{ (Int) (as<Char>() <=> other.as<Char>())._Value };

        default:
            PANIC( "[", type(), "] <=> [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRING:
    {
        switch ( other.type() )
        {
        case ValueType::STRING:
            return ValueRef{ (Int) (as<String>() <=> other.as<String>())._Value };

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

            return ValueRef{ (Int) (left.key_id <=> right.key_id)._Value };
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
            PANIC( "struct [", IDSystem::get( left.parent->id ), "] <=> struct [", other.type(), "] not supported" );

        op->self_vals.resize( 2 );
        op->self_vals[0] = *this;
        op->self_vals[1] = other;

        return engine.handle_func( *op, op->self_vals.data(), (Int) op->self_vals.size() );
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
                    return ValueRef{ cmpr_res };
            }
            return ValueRef{ (Int) (left.size() <=> right.size())._Value };
        }

        default:
            PANIC( "[", type(), "] <=> [", other.type(), "] not supported" );
        }
    }

    default:
        PANIC( "[", type(), "] <=> [", other.type(), "] not supported" );
    }
}

dawn::ValueRef dawn::ValueRef::op_eq( Engine& engine, ValueRef const& other ) const
{
    auto result = op_cmpr( engine, other );
    result.m_regref.value().emplace<Bool>( result.as<Int>() == 0 );
    return result;
}

dawn::ValueRef dawn::ValueRef::op_neq( Engine& engine, ValueRef const& other ) const
{
    auto result = op_cmpr( engine, other );
    result.m_regref.value().emplace<Bool>( result.as<Int>() != 0 );
    return result;
}

dawn::ValueRef dawn::ValueRef::op_less( Engine& engine, ValueRef const& other ) const
{
    auto result = op_cmpr( engine, other );
    result.m_regref.value().emplace<Bool>( result.as<Int>() < 0 );
    return result;
}

dawn::ValueRef dawn::ValueRef::op_great( Engine& engine, ValueRef const& other ) const
{
    auto result = op_cmpr( engine, other );
    result.m_regref.value().emplace<Bool>( result.as<Int>() > 0 );
    return result;
}

dawn::ValueRef dawn::ValueRef::op_lesseq( Engine& engine, ValueRef const& other ) const
{
    auto result = op_cmpr( engine, other );
    result.m_regref.value().emplace<Bool>( result.as<Int>() <= 0 );
    return result;
}

dawn::ValueRef dawn::ValueRef::op_greateq( Engine& engine, ValueRef const& other ) const
{
    auto result = op_cmpr( engine, other );
    result.m_regref.value().emplace<Bool>( result.as<Int>() >= 0 );
    return result;
}

dawn::ValueRef dawn::ValueRef::un_not( Engine& engine ) const
{
    return ValueRef{ !to_bool( engine ) };
}

dawn::ValueRef dawn::ValueRef::op_and( Engine& engine, ValueRef const& other ) const
{
    return ValueRef{ to_bool( engine ) && other.to_bool( engine ) };
}

dawn::ValueRef dawn::ValueRef::op_or( Engine& engine, ValueRef const& other ) const
{
    return ValueRef{ to_bool( engine ) || other.to_bool( engine ) };
}

dawn::ValueRef dawn::ValueRef::op_range( Engine& engine, ValueRef const& other ) const
{
    RangeVal result;
    result.start_incl = to_int( engine );
    result.end_excl = other.to_int( engine );
    return ValueRef{ result };
}

dawn::Bool dawn::ValueRef::to_bool( Engine& engine ) const
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

        method->self_vals.resize( 1 );
        method->self_vals[0] = *this;

        return engine.handle_func( *method, method->self_vals.data(), (Int) method->self_vals.size() ).to_bool( engine );
    }

    default:
        PANIC( "can't convert [", type(), "] to bool" );
    }
}

dawn::Int dawn::ValueRef::to_int( Engine& engine ) const
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
        throw ValueRef{ dawn::format( "string \"", as<String>(), "\" to int failed" ) };
    }

    case ValueType::STRUCT:
    {
        auto& left = as<StructVal>();
        Function* method = left.get_method( _to_int, true );
        if ( !method )
            PANIC( "can't convert struct [", IDSystem::get( left.parent->id ), "] to int" );

        method->self_vals.resize( 1 );
        method->self_vals[0] = *this;

        return engine.handle_func( *method, method->self_vals.data(), (Int) method->self_vals.size() ).to_int( engine );
    }

    default:
        PANIC( "can't convert [", type(), "] to int" );
    }
}

dawn::Float dawn::ValueRef::to_float( Engine& engine ) const
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
        throw ValueRef{ dawn::format( "string \"", as<String>(), "\" to float failed" ) };
    }

    case ValueType::STRUCT:
    {
        auto& left = as<StructVal>();
        Function* method = left.get_method( _to_float, true );
        if ( !method )
            PANIC( "can't convert struct [", IDSystem::get( left.parent->id ), "] to float" );

        method->self_vals.resize( 1 );
        method->self_vals[0] = *this;

        return engine.handle_func( *method, method->self_vals.data(), (Int) method->self_vals.size() ).to_float( engine );
    }

    default:
        PANIC( "can't convert [", type(), "] to float" );
    }
}

dawn::Char dawn::ValueRef::to_char( Engine& engine ) const
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

        method->self_vals.resize( 1 );
        method->self_vals[0] = *this;

        return engine.handle_func( *method, method->self_vals.data(), (Int) method->self_vals.size() ).to_char( engine );
    }

    default:
        PANIC( "can't convert [", type(), "] to char" );
    }
}

dawn::String dawn::ValueRef::to_string( Engine& engine ) const
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
            return format( IDSystem::get( func.self_vals[0].as<StructVal>().parent->id ), "::", IDSystem::get( func.id ), "()" );
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

        method->self_vals.resize( 1 );
        method->self_vals[0] = *this;

        return engine.handle_func( *method, method->self_vals.data(), (Int) method->self_vals.size() ).to_string( engine );
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

void dawn::ValueRef::reapply_kind()
{
    auto& val = m_regref.value();
    switch ( val.type() )
    {
    case ValueType::STRUCT:
    {
        for ( auto& [_, member] : val.as<StructVal>().members )
        {
            member.m_kind = this->m_kind;
            member.reapply_kind();
        }
        break;
    }

    case ValueType::ARRAY:
    {
        for ( auto& element : val.as<ArrayVal>().data )
        {
            element.m_kind = this->m_kind;
            element.reapply_kind();
        }
        break;
    }
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
