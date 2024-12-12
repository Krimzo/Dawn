#include "values.h"


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

dawn::Value::Value( Bool value )
{
    store<Bool>( value );
}

dawn::Value::Value( Int value )
{
    store<Int>( value );
}

dawn::Value::Value( Float value )
{
    store<Float>( value );
}

dawn::Value::Value( Char value )
{
    store<Char>( value );
}

dawn::Value::Value( StringRef const& value )
{
    store<String>( value );
}

dawn::Value::Value( EnumVal const& value )
{
    store<EnumVal>( value );
}

dawn::Value::Value( StructVal const& value )
{
    store<StructVal>( value );
}

dawn::Value::Value( ArrayVal const& value )
{
    store<ArrayVal>( value );
}

dawn::Value::Value( RangeVal const& value )
{
    store<RangeVal>( value );
}

dawn::Value dawn::Value::operator+() const
{
    switch ( type() )
    {
    case ValueType::INT:
        return Value{ +as<Int>() };

    case ValueType::FLOAT:
        return Value{ +as<Float>() };

    default:
        PANIC( "+ [", type(), "] not supported" );
    }
}

dawn::Value dawn::Value::operator-() const
{
    switch ( type() )
    {
    case ValueType::INT:
        return Value{ -as<Int>() };

    case ValueType::FLOAT:
        return Value{ -as<Float>() };

    default:
        PANIC( "- [", type(), "] not supported" );
    }
}

dawn::Value dawn::Value::operator+( Value const& other ) const
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

    default:
        PANIC( "[", type(), "] + [", other.type(), "] not supported" );
    }
}

dawn::Value dawn::Value::operator-( Value const& other ) const
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

    default:
        PANIC( "[", type(), "] - [", other.type(), "] not supported" );
    }
}

dawn::Value dawn::Value::operator*( Value const& other ) const
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

    default:
        PANIC( "[", type(), "] * [", other.type(), "] not supported" );
    }
}

dawn::Value dawn::Value::operator/( Value const& other ) const
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

    default:
        PANIC( "[", type(), "] / [", other.type(), "] not supported" );
    }
}

dawn::Value dawn::Value::operator^( Value const& other ) const
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

    default:
        PANIC( "[", type(), "] ^ [", other.type(), "] not supported" );
    }
}

dawn::Value dawn::Value::operator%( Value const& other ) const
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

    default:
        PANIC( "[", type(), "] % [", other.type(), "] not supported" );
    }
}

dawn::Value dawn::Value::operator==( Value const& other ) const
{
    switch ( type() )
    {
    case ValueType::BOOL:
        return to_bool() == other.to_bool();

    case ValueType::INT:
        return to_int() == other.to_int();

    case ValueType::FLOAT:
        return to_float() == other.to_float();

    case ValueType::CHAR:
        return to_char() == other.to_char();

    case ValueType::STRING:
        return to_string() == other.to_string();

    case ValueType::ENUM:
    {
        if ( other.type() != ValueType::ENUM )
            PANIC( "[", type(), "] == [", other.type(), "] not supported" );

        auto& left = as<EnumVal>();
        auto& right = other.as<EnumVal>();
        if ( left.parent != right.parent )
            PANIC( "enum [", left.parent->name, "] == enum [", right.parent->name, "] not supported" );

        return left.key == right.key;
    }

    default:
        PANIC( "[", type(), "] == [", other.type(), "] not supported" );
    }
}

dawn::Value dawn::Value::operator!=( Value const& other ) const
{
    switch ( type() )
    {
    case ValueType::BOOL:
        return to_bool() != other.to_bool();

    case ValueType::INT:
        return to_int() != other.to_int();

    case ValueType::FLOAT:
        return to_float() != other.to_float();

    case ValueType::CHAR:
        return to_char() != other.to_char();

    case ValueType::STRING:
        return to_string() != other.to_string();

    case ValueType::ENUM:
    {
        if ( other.type() != ValueType::ENUM )
            PANIC( "[", type(), "] != [", other.type(), "] not supported" );

        auto& left = as<EnumVal>();
        auto& right = other.as<EnumVal>();
        if ( left.parent != right.parent )
            PANIC( "enum [", left.parent->name, "] != enum [", right.parent->name, "] not supported" );

        return left.key != right.key;
    }

    default:
        PANIC( "[", type(), "] != [", other.type(), "] not supported" );
    }
}

dawn::Value dawn::Value::operator<( Value const& other ) const
{
    switch ( type() )
    {
    case ValueType::BOOL:
        return to_bool() < other.to_bool();

    case ValueType::INT:
        return to_int() < other.to_int();

    case ValueType::FLOAT:
        return to_float() < other.to_float();

    case ValueType::CHAR:
        return to_char() < other.to_char();

    case ValueType::STRING:
        return to_string() < other.to_string();

    default:
        PANIC( "[", type(), "] < [", other.type(), "] not supported" );
    }
}

dawn::Value dawn::Value::operator>( Value const& other ) const
{
    switch ( type() )
    {
    case ValueType::BOOL:
        return to_bool() > other.to_bool();

    case ValueType::INT:
        return to_int() > other.to_int();

    case ValueType::FLOAT:
        return to_float() > other.to_float();

    case ValueType::CHAR:
        return to_char() > other.to_char();

    case ValueType::STRING:
        return to_string() > other.to_string();

    default:
        PANIC( "[", type(), "] > [", other.type(), "] not supported" );
    }
}

dawn::Value dawn::Value::operator<=( Value const& other ) const
{
    switch ( type() )
    {
    case ValueType::BOOL:
        return to_bool() <= other.to_bool();

    case ValueType::INT:
        return to_int() <= other.to_int();

    case ValueType::FLOAT:
        return to_float() <= other.to_float();

    case ValueType::CHAR:
        return to_char() <= other.to_char();

    case ValueType::STRING:
        return to_string() <= other.to_string();

    default:
        PANIC( "[", type(), "] <= [", other.type(), "] not supported" );
    }
}

dawn::Value dawn::Value::operator>=( Value const& other ) const
{
    switch ( type() )
    {
    case ValueType::BOOL:
        return to_bool() >= other.to_bool();

    case ValueType::INT:
        return to_int() >= other.to_int();

    case ValueType::FLOAT:
        return to_float() >= other.to_float();

    case ValueType::CHAR:
        return to_char() >= other.to_char();

    case ValueType::STRING:
        return to_string() >= other.to_string();

    default:
        PANIC( "[", type(), "] >= [", other.type(), "] not supported" );
    }
}

dawn::Value dawn::Value::operator!() const
{
    return !to_bool();
}

dawn::Value dawn::Value::operator&&( Value const& other ) const
{
    return to_bool() && other.to_bool();
}

dawn::Value dawn::Value::operator||( Value const& other ) const
{
    return to_bool() || other.to_bool();
}

dawn::Value dawn::Value::operator~() const
{
    return Value{ 0ll } >> *this;
}

dawn::Value dawn::Value::operator>>( Value const& other ) const
{
    RangeVal result;
    result.start_incl = to_int();
    result.end_excl = other.to_int();
    return Value{ result };
}

dawn::Bool dawn::Value::to_bool() const
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

    default:
        PANIC( "Cannot convert ", type(), " to bool" );
    }
}

dawn::Int dawn::Value::to_int() const
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
        auto optres = parse_int( as<String>() );
        if ( !optres )
            PANIC( "string \"", as<String>(), "\" to int failed" );
        return *optres;
    }

    default:
        PANIC( "Cannot convert ", type(), " to int" );
    }
}

dawn::Float dawn::Value::to_float() const
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
        auto optres = parse_float( as<String>() );
        if ( !optres )
            PANIC( "string \"", as<String>(), "\" to float failed" );
        return *optres;
    }

    default:
        PANIC( "Cannot convert ", type(), " to float" );
    }
}

dawn::Char dawn::Value::to_char() const
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

    default:
        PANIC( "Cannot convert ", type(), " to char" );
    }
}

dawn::String dawn::Value::to_string() const
{
    switch ( type() )
    {
    case ValueType::NOTHING:
        return String{};

    case ValueType::BOOL:
        return String{ as<Bool>() ? kw_true : kw_false };

    case ValueType::INT:
        return std::to_wstring( as<Int>() );

    case ValueType::FLOAT:
    {
        String result = format( as<Float>() );
        if ( std::to_wstring( (Int) as<Float>() ) == result )
            result += L".0";
        return result;
    }

    case ValueType::CHAR:
        return String( 1, as<Char>() );

    case ValueType::STRING:
        return as<String>();

    case ValueType::ENUM:
    {
        auto& val = as<EnumVal>();
        return val.parent->name + L"{" + val.key + L"}";
    }

    case ValueType::STRUCT:
    {
        auto& val = as<StructVal>();
        if ( val.members.empty() )
            return val.parent->name + L"{}";

        StringStream stream;
        stream << val.parent->name << L"{ ";
        for ( auto it = val.members.begin(); it != val.members.end(); it++ )
        {
            auto& [key, value] = *it;
            stream << key << L": " << value.value().to_string();
            Map<String, ValueRef>::const_iterator next_it = it;
            ++next_it;
            stream << (next_it == val.members.end() ? L" }" : L", ");
        }
        return stream.str();
    }

    case ValueType::ARRAY:
    {
        auto& val = as<ArrayVal>();
        if ( val.data.empty() )
            return L"[]";

        StringStream stream;
        stream << L"[";
        for ( Int i = 0; i < (Int) val.data.size() - 1; i++ )
            stream << val.data[i].value().to_string() << L", ";
        stream << val.data.back().value().to_string() << L"]";

        return stream.str();
    }

    case ValueType::RANGE:
    {
        auto& val = as<RangeVal>();
        return format( val.start_incl, op_range, val.end_excl );
    }

    default:
        PANIC( "Cannot convert ", type(), " to string" );
    }
}

static dawn::Memory<dawn::Value> _MEMORY{ 2048 };

dawn::ValueRef::ValueRef( Value const& value, ValueKind kind )
    : m_regref( _MEMORY.new_register() ), m_kind( kind )
{
    m_regref.value() = value;
    reapply_kind();
}

dawn::ValueKind dawn::ValueRef::kind() const
{
    return m_kind;
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

std::wostream& dawn::operator<<( std::wostream& stream, ValueType type )
{
    switch ( type )
    {
    case ValueType::NOTHING: stream << tp_nothing; break;
    case ValueType::BOOL: stream << tp_bool; break;
    case ValueType::INT: stream << tp_int; break;
    case ValueType::FLOAT: stream << tp_float; break;
    case ValueType::CHAR: stream << tp_char; break;
    case ValueType::STRING: stream << tp_string; break;
    case ValueType::ENUM: stream << tp_enum; break;
    case ValueType::STRUCT: stream << tp_struct; break;
    case ValueType::ARRAY: stream << tp_array; break;
    case ValueType::RANGE: stream << tp_range; break;
    }
    return stream;
}
