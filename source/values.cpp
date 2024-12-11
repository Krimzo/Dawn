#include "values.h"


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

dawn::Value::Value()
{}

dawn::Value::Value( Bool value )
{
    m_value.emplace<Bool>( value );
}

dawn::Value::Value( Int value )
{
    m_value.emplace<Int>( value );
}

dawn::Value::Value( Float value )
{
    m_value.emplace<Float>( value );
}

dawn::Value::Value( Char value )
{
    m_value.emplace<Char>( value );
}

dawn::Value::Value( StringRef const& value )
{
    m_value.emplace<String>( value );
}

dawn::Value::Value( EnumVal const& value )
{
    m_value.emplace<EnumVal>( value );
}

dawn::Value::Value( StructVal const& value )
{
    m_value.emplace<StructVal>( value );
}

dawn::Value::Value( ArrayVal const& value )
{
    m_value.emplace<ArrayVal>( value );
}

dawn::Value::Value( RangeVal const& value )
{
    m_value.emplace<RangeVal>( value );
}

dawn::Value dawn::Value::operator+() const
{
    switch ( type() )
    {
    case ValueType::INT:
        return Value{ +std::get<Int>( m_value ) };

    case ValueType::FLOAT:
        return Value{ +std::get<Float>( m_value ) };

    default:
        PANIC( "+ [", type(), "] not supported" );
    }
}

dawn::Value dawn::Value::operator-() const
{
    switch ( type() )
    {
    case ValueType::INT:
        return Value{ -std::get<Int>( m_value ) };

    case ValueType::FLOAT:
        return Value{ -std::get<Float>( m_value ) };

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
            return Value{ std::get<Int>( m_value ) + std::get<Int>( other.m_value ) };

        case ValueType::FLOAT:
            return Value{ std::get<Int>( m_value ) + std::get<Float>( other.m_value ) };

        default:
            PANIC( "[", type(), "] + [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ std::get<Float>( m_value ) + std::get<Int>( other.m_value ) };

        case ValueType::FLOAT:
            return Value{ std::get<Float>( m_value ) + std::get<Float>( other.m_value ) };

        default:
            PANIC( "[", type(), "] + [", other.type(), "] not supported" );
        }
    }

    case ValueType::STRING:
    {
        switch ( other.type() )
        {
        case ValueType::STRING:
            return Value{ std::get<String>( m_value ) + std::get<String>( other.m_value ) };

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
            result.data.insert( result.data.end(), std::get<ArrayVal>( m_value ).data.begin(), std::get<ArrayVal>( m_value ).data.end() );
            result.data.insert( result.data.end(), std::get<ArrayVal>( other.m_value ).data.begin(), std::get<ArrayVal>( other.m_value ).data.end() );
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
            return Value{ std::get<Int>( m_value ) - std::get<Int>( other.m_value ) };

        case ValueType::FLOAT:
            return Value{ std::get<Int>( m_value ) - std::get<Float>( other.m_value ) };

        default:
            PANIC( "[", type(), "] - [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ std::get<Float>( m_value ) - std::get<Int>( other.m_value ) };

        case ValueType::FLOAT:
            return Value{ std::get<Float>( m_value ) - std::get<Float>( other.m_value ) };

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
            return Value{ std::get<Int>( m_value ) * std::get<Int>( other.m_value ) };

        case ValueType::FLOAT:
            return Value{ std::get<Int>( m_value ) * std::get<Float>( other.m_value ) };

        default:
            PANIC( "[", type(), "] * [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ std::get<Float>( m_value ) * std::get<Int>( other.m_value ) };

        case ValueType::FLOAT:
            return Value{ std::get<Float>( m_value ) * std::get<Float>( other.m_value ) };

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
            return Value{ std::get<Int>( m_value ) / std::get<Int>( other.m_value ) };

        case ValueType::FLOAT:
            return Value{ std::get<Int>( m_value ) / std::get<Float>( other.m_value ) };

        default:
            PANIC( "[", type(), "] / [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ std::get<Float>( m_value ) / std::get<Int>( other.m_value ) };

        case ValueType::FLOAT:
            return Value{ std::get<Float>( m_value ) / std::get<Float>( other.m_value ) };

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
            return Value{ (Int) std::pow( std::get<Int>( m_value ), std::get<Int>( other.m_value ) ) };

        case ValueType::FLOAT:
            return Value{ std::pow( std::get<Int>( m_value ), std::get<Float>( other.m_value ) ) };

        default:
            PANIC( "[", type(), "] ^ [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ std::pow( std::get<Float>( m_value ), std::get<Int>( other.m_value ) ) };

        case ValueType::FLOAT:
            return Value{ std::pow( std::get<Float>( m_value ), std::get<Float>( other.m_value ) ) };

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
            return Value{ std::get<Int>( m_value ) % std::get<Int>( other.m_value ) };

        case ValueType::FLOAT:
            return Value{ mymod( (Float) std::get<Int>( m_value ), std::get<Float>( other.m_value ) ) };

        default:
            PANIC( "[", type(), "] % [", other.type(), "] not supported" );
        }
    }

    case ValueType::FLOAT:
    {
        switch ( other.type() )
        {
        case ValueType::INT:
            return Value{ mymod( std::get<Float>( m_value ), (Float) std::get<Int>( other.m_value ) ) };

        case ValueType::FLOAT:
            return Value{ mymod( std::get<Float>( m_value ), std::get<Float>( other.m_value ) ) };

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
        return std::get<Bool>( m_value );

    case ValueType::INT:
        return (Bool) std::get<Int>( m_value );

    case ValueType::FLOAT:
        return (Bool) std::get<Float>( m_value );

    case ValueType::CHAR:
        return (Bool) std::get<Char>( m_value );

    case ValueType::STRING:
        return std::get<String>( m_value ) == kw_true;

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
        return (Int) std::get<Bool>( m_value );

    case ValueType::INT:
        return std::get<Int>( m_value );

    case ValueType::FLOAT:
        return (Int) std::get<Float>( m_value );

    case ValueType::CHAR:
        return (Int) std::get<Char>( m_value );

    case ValueType::STRING:
    {
        auto optres = parse_int( std::get<String>( m_value ) );
        if ( !optres )
            PANIC( "string \"", std::get<String>( m_value ), "\" to int failed" );
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
        return (Float) std::get<Bool>( m_value );

    case ValueType::INT:
        return (Float) std::get<Int>( m_value );

    case ValueType::FLOAT:
        return std::get<Float>( m_value );

    case ValueType::CHAR:
        return (Float) std::get<Char>( m_value );

    case ValueType::STRING:
    {
        auto optres = parse_float( std::get<String>( m_value ) );
        if ( !optres )
            PANIC( "string \"", std::get<String>( m_value ), "\" to float failed" );
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
        return std::get<Bool>( m_value ) ? kw_true[0] : kw_false[0];

    case ValueType::INT:
        return (Char) std::get<Int>( m_value );

    case ValueType::FLOAT:
        return (Char) std::get<Float>( m_value );

    case ValueType::CHAR:
        return std::get<Char>( m_value );

    case ValueType::STRING:
        return std::get<String>( m_value )[0];

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
        return String{ std::get<Bool>( m_value ) ? kw_true : kw_false };

    case ValueType::INT:
        return std::to_wstring( std::get<Int>( m_value ) );

    case ValueType::FLOAT:
    {
        String result = format( std::get<Float>( m_value ) );
        if ( std::to_wstring( (Int) std::get<Float>( m_value ) ) == result )
            result += L".0";
        return result;
    }

    case ValueType::CHAR:
        return String( 1, std::get<Char>( m_value ) );

    case ValueType::STRING:
        return std::get<String>( m_value );

    case ValueType::ENUM:
    {
        auto& val = std::get<EnumVal>( m_value );
        return val.parent->name + L"{" + val.key + L"}";
    }

    case ValueType::STRUCT:
    {
        auto& val = std::get<StructVal>( m_value );
        if ( val.members.empty() )
            return val.parent->name + L"{}";

        StringStream stream;
        stream << val.parent->name << L"{ ";
        for ( auto it = val.members.begin(); it != val.members.end(); it++ )
        {
            auto& [key, value] = *it;
            stream << key << L": " << value.value().to_string();
            Map<String, ValueBox>::const_iterator next_it = it;
            ++next_it;
            stream << (next_it == val.members.end() ? L" }" : L", ");
        }
        return stream.str();
    }

    case ValueType::ARRAY:
    {
        auto& val = std::get<ArrayVal>( m_value );
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
        auto& val = std::get<RangeVal>( m_value );
        return format( val.start_incl, op_range, val.end_excl );
    }

    default:
        PANIC( "Cannot convert ", type(), " to string" );
    }
}

dawn::ValueBox::ValueBox()
    : ValueBox( Value{} )
{}

dawn::ValueBox::ValueBox( Value const& value, ValueKind kind )
{
    m_value_ref = std::make_shared<Value>( value );

    m_kind = ValueKind::VAR;
    set_value( value );

    m_kind = kind;
    reapply_kind();
}

dawn::Value const& dawn::ValueBox::value() const
{
    return *m_value_ref;
}

void dawn::ValueBox::set_value( Value const& value )
{
    if ( m_kind == ValueKind::LET )
        PANIC( "Cannot set value of a let variable" );

    (*m_value_ref) = value;
    reapply_kind();
}

void dawn::ValueBox::reapply_kind()
{
    switch ( value().type() )
    {
    case ValueType::STRUCT:
    {
        auto& val = (*m_value_ref).as<StructVal>();
        for ( auto& [_, member] : val.members )
        {
            member.m_kind = this->m_kind;
            member.reapply_kind();
        }
        break;
    }

    case ValueType::ARRAY:
    {
        auto& val = (*m_value_ref).as<ArrayVal>();
        for ( auto& value : val.data )
        {
            value.m_kind = this->m_kind;
            value.reapply_kind();
        }
        break;
    }
    }
}
