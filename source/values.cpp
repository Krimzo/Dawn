#include "values.h"
#include "engine.h"


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
    : m_regref( get_global_memory().value_memory.new_register() ), m_kind( kind )
{
    m_regref.value().store<Bool>( value );
}

dawn::ValueRef::ValueRef( Int value, ValueKind kind )
    : m_regref( get_global_memory().value_memory.new_register() ), m_kind( kind )
{
    m_regref.value().store<Int>( value );
}

dawn::ValueRef::ValueRef( Float value, ValueKind kind )
    : m_regref( get_global_memory().value_memory.new_register() ), m_kind( kind )
{
    m_regref.value().store<Float>( value );
}

dawn::ValueRef::ValueRef( Char value, ValueKind kind )
    : m_regref( get_global_memory().value_memory.new_register() ), m_kind( kind )
{
    m_regref.value().store<Char>( value );
}

dawn::ValueRef::ValueRef( String value, ValueKind kind )
    : m_regref( get_global_memory().value_memory.new_register() ), m_kind( kind )
{
    m_regref.value().store<String>( std::move( value ) );
}

dawn::ValueRef::ValueRef( Function const& value, ValueKind kind )
    : m_regref( get_global_memory().value_memory.new_register() ), m_kind( kind )
{
    m_regref.value().store<Function>( value );
}

dawn::ValueRef::ValueRef( EnumVal const& value, ValueKind kind )
    : m_regref( get_global_memory().value_memory.new_register() ), m_kind( kind )
{
    m_regref.value().store<EnumVal>( value );
}

dawn::ValueRef::ValueRef( StructVal const& value, ValueKind kind )
    : m_regref( get_global_memory().value_memory.new_register() ), m_kind( kind )
{
    m_regref.value().store<StructVal>( value );
    reapply_kind();
}

dawn::ValueRef::ValueRef( ArrayVal const& value, ValueKind kind )
    : m_regref( get_global_memory().value_memory.new_register() ), m_kind( kind )
{
    m_regref.value().store<ArrayVal>( value );
    reapply_kind();
}

dawn::ValueRef::ValueRef( RangeVal const& value, ValueKind kind )
    : m_regref( get_global_memory().value_memory.new_register() ), m_kind( kind )
{
    m_regref.value().store<RangeVal>( value );
}

dawn::ValueRef::ValueRef( Value const& value, ValueKind kind )
    : m_regref( get_global_memory().value_memory.new_register() ), m_kind( kind )
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
        PANIC( "Cannot set value of a let variable" );

    m_regref.value() = value;
    reapply_kind();
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
        Function* op = left.get_method( engine.predefines.__add.get( engine.id_system ), true );
        if ( !op )
            PANIC( "+ struct [", left.parent->name, "] not supported" );

        ValueRef retval;
        op->self_val.resize( 1 );
        op->self_val[0] = *this;

        engine.handle_func( *op, op->self_val, retval );
        return retval;
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
        Function* op = left.get_method( engine.predefines.__sub.get( engine.id_system ), true );
        if ( !op )
            PANIC( "- struct [", left.parent->name, "] not supported" );

        ValueRef retval;
        op->self_val.resize( 1 );
        op->self_val[0] = *this;

        engine.handle_func( *op, op->self_val, retval );
        return retval;
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
        Function* op = left.get_method( engine.predefines.__add.get( engine.id_system ), false );
        if ( !op )
            PANIC( "struct [", left.parent->name, "] + struct [", other.type(), "] not supported" );

        ValueRef retval;
        op->self_val.resize( 2 );
        op->self_val[0] = *this;
        op->self_val[1] = other;

        engine.handle_func( *op, op->self_val, retval );
        return retval;
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
        Function* op = left.get_method( engine.predefines.__sub.get( engine.id_system ), false );
        if ( !op )
            PANIC( "struct [", left.parent->name, "] - struct [", other.type(), "] not supported" );

        ValueRef retval;
        op->self_val.resize( 2 );
        op->self_val[0] = *this;
        op->self_val[1] = other;

        engine.handle_func( *op, op->self_val, retval );
        return retval;
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
        Function* op = left.get_method( engine.predefines.__mul.get( engine.id_system ), false );
        if ( !op )
            PANIC( "struct [", left.parent->name, "] * struct [", other.type(), "] not supported" );

        ValueRef retval;
        op->self_val.resize( 2 );
        op->self_val[0] = *this;
        op->self_val[1] = other;

        engine.handle_func( *op, op->self_val, retval );
        return retval;
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
        Function* op = left.get_method( engine.predefines.__div.get( engine.id_system ), false );
        if ( !op )
            PANIC( "struct [", left.parent->name, "] / struct [", other.type(), "] not supported" );

        ValueRef retval;
        op->self_val.resize( 2 );
        op->self_val[0] = *this;
        op->self_val[1] = other;

        engine.handle_func( *op, op->self_val, retval );
        return retval;
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
        Function* op = left.get_method( engine.predefines.__pow.get( engine.id_system ), false );
        if ( !op )
            PANIC( "struct [", left.parent->name, "] ^ struct [", other.type(), "] not supported" );

        ValueRef retval;
        op->self_val.resize( 2 );
        op->self_val[0] = *this;
        op->self_val[1] = other;

        engine.handle_func( *op, op->self_val, retval );
        return retval;
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
        Function* op = left.get_method( engine.predefines.__mod.get( engine.id_system ), false );
        if ( !op )
            PANIC( "struct [", left.parent->name, "] % struct [", other.type(), "] not supported" );

        ValueRef retval;
        op->self_val.resize( 2 );
        op->self_val[0] = *this;
        op->self_val[1] = other;

        engine.handle_func( *op, op->self_val, retval );
        return retval;
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
                PANIC( "enum [", left.parent->name, "] <=> enum [", right.parent->name, "] not supported" );

            return ValueRef{ (Int) (left.key.str_id <=> right.key.str_id)._Value };
        }

        default:
            PANIC( "[", type(), "] <=> [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRUCT:
    {
        auto& left = as<StructVal>();
        Function* op = left.get_method( engine.predefines.__cmpr.get( engine.id_system ), false );
        if ( !op )
            PANIC( "struct [", left.parent->name, "] <=> struct [", other.type(), "] not supported" );

        ValueRef retval;
        op->self_val.resize( 2 );
        op->self_val[0] = *this;
        op->self_val[1] = other;

        engine.handle_func( *op, op->self_val, retval );
        return retval;
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
    result.m_regref.value().store<Bool>( result.as<Int>() == 0 );
    return result;
}

dawn::ValueRef dawn::ValueRef::op_neq( Engine& engine, ValueRef const& other ) const
{
    auto result = op_cmpr( engine, other );
    result.m_regref.value().store<Bool>( result.as<Int>() != 0 );
    return result;
}

dawn::ValueRef dawn::ValueRef::op_less( Engine& engine, ValueRef const& other ) const
{
    auto result = op_cmpr( engine, other );
    result.m_regref.value().store<Bool>( result.as<Int>() < 0 );
    return result;
}

dawn::ValueRef dawn::ValueRef::op_great( Engine& engine, ValueRef const& other ) const
{
    auto result = op_cmpr( engine, other );
    result.m_regref.value().store<Bool>( result.as<Int>() > 0 );
    return result;
}

dawn::ValueRef dawn::ValueRef::op_lesseq( Engine& engine, ValueRef const& other ) const
{
    auto result = op_cmpr( engine, other );
    result.m_regref.value().store<Bool>( result.as<Int>() <= 0 );
    return result;
}

dawn::ValueRef dawn::ValueRef::op_greateq( Engine& engine, ValueRef const& other ) const
{
    auto result = op_cmpr( engine, other );
    result.m_regref.value().store<Bool>( result.as<Int>() >= 0 );
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
        Function* method = left.get_method( engine.predefines._to_bool.get( engine.id_system ), true );
        if ( !method )
            PANIC( "Cannot convert struct [", left.parent->name, "] to bool" );

        ValueRef retval;
        method->self_val.resize( 1 );
        method->self_val[0] = *this;

        engine.handle_func( *method, method->self_val, retval );
        return retval.to_bool( engine );
    }

    default:
        PANIC( "Cannot convert [", type(), "] to bool" );
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
        Function* method = left.get_method( engine.predefines._to_int.get( engine.id_system ), true );
        if ( !method )
            PANIC( "Cannot convert struct [", left.parent->name, "] to int" );

        ValueRef retval;
        method->self_val.resize( 1 );
        method->self_val[0] = *this;

        engine.handle_func( *method, method->self_val, retval );
        return retval.to_int( engine );
    }

    default:
        PANIC( "Cannot convert [", type(), "] to int" );
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
        Function* method = left.get_method( engine.predefines._to_float.get( engine.id_system ), true );
        if ( !method )
            PANIC( "Cannot convert struct [", left.parent->name, "] to float" );

        ValueRef retval;
        method->self_val.resize( 1 );
        method->self_val[0] = *this;

        engine.handle_func( *method, method->self_val, retval );
        return retval.to_float( engine );
    }

    default:
        PANIC( "Cannot convert [", type(), "] to float" );
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
        Function* method = left.get_method( engine.predefines._to_char.get( engine.id_system ), true );
        if ( !method )
            PANIC( "Cannot convert struct [", left.parent->name, "] to char" );

        ValueRef retval;
        method->self_val.resize( 1 );
        method->self_val[0] = *this;

        engine.handle_func( *method, method->self_val, retval );
        return retval.to_char( engine );
    }

    default:
        PANIC( "Cannot convert [", type(), "] to char" );
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
        return format( func.parent->name, "::", func.name, "()" );
    }

    case ValueType::ENUM:
        return as<EnumVal>().key.str_id;

    case ValueType::STRUCT:
    {
        auto& left = as<StructVal>();
        Function* method = left.get_method( engine.predefines._to_string.get( engine.id_system ), true );
        if ( !method )
            return format( left.parent->name, "{}" );

        ValueRef retval;
        method->self_val.resize( 1 );
        method->self_val[0] = *this;

        engine.handle_func( *method, method->self_val, retval );
        return retval.to_string( engine );
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
        PANIC( "Cannot convert [", type(), "] to string" );
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
