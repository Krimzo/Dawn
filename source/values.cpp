#include "values.h"


// nothing
dawn::StringRef const& dawn::NothingValue::type() const
{
    return tp_nothing;
}

dawn::RawValue dawn::NothingValue::clone() const
{
    return make_nothing_value();
}

dawn::Bool dawn::NothingValue::to_bool() const
{
    return {};
}

dawn::Int dawn::NothingValue::to_int() const
{
    return {};
}

dawn::Float dawn::NothingValue::to_float() const
{
    return {};
}

dawn::Char dawn::NothingValue::to_char() const
{
    return {};
}

dawn::String dawn::NothingValue::to_string() const
{
    return {};
}

// bool
dawn::StringRef const& dawn::BoolValue::type() const
{
    return tp_bool;
}

dawn::RawValue dawn::BoolValue::clone() const
{
    return make_bool_value( value );
}

dawn::Int dawn::BoolValue::operator<=>( Value const& other ) const
{
    if ( other.type() == tp_bool )
    {
        auto result = value <=> other.to_bool();
        return result._Value;
    }

    return Value::operator<=>( other );
}

dawn::Bool dawn::BoolValue::to_bool() const
{
    return value;
}

dawn::Int dawn::BoolValue::to_int() const
{
    return (Int) value;
}

dawn::Float dawn::BoolValue::to_float() const
{
    return (Float) value;
}

dawn::Char dawn::BoolValue::to_char() const
{
    if ( value )
        return L't';
    return L'f';
}

dawn::String dawn::BoolValue::to_string() const
{
    if ( value )
        return L"true";
    return L"false";
}

// int
dawn::StringRef const& dawn::IntValue::type() const
{
    return tp_int;
}

dawn::RawValue dawn::IntValue::clone() const
{
    return make_int_value( value );
}

dawn::RawValue dawn::IntValue::operator-() const
{
    return make_int_value( -value );
}

dawn::RawValue dawn::IntValue::operator+( Value const& other ) const
{
    if ( other.type() == tp_int )
        return make_int_value( value + other.to_int() );

    if ( other.type() == tp_float )
        return make_float_value( value + other.to_float() );

    return Value::operator+( other );
}

dawn::RawValue dawn::IntValue::operator-( Value const& other ) const
{
    if ( other.type() == tp_int )
        return make_int_value( value - other.to_int() );

    if ( other.type() == tp_float )
        return make_float_value( value - other.to_float() );

    return Value::operator-( other );
}

dawn::RawValue dawn::IntValue::operator*( Value const& other ) const
{
    if ( other.type() == tp_int )
        return make_int_value( value * other.to_int() );

    if ( other.type() == tp_float )
        return make_float_value( value * other.to_float() );

    return Value::operator*( other );
}

dawn::RawValue dawn::IntValue::operator/( Value const& other ) const
{
    if ( other.type() == tp_int )
        return make_int_value( value / other.to_int() );

    if ( other.type() == tp_float )
        return make_float_value( value / other.to_float() );

    return Value::operator/( other );
}

dawn::RawValue dawn::IntValue::operator^( Value const& other ) const
{
    if ( other.type() == tp_int )
        return make_int_value( (Int) std::pow( value, other.to_int() ) );

    if ( other.type() == tp_float )
        return make_float_value( (Float) std::pow( value, other.to_float() ) );

    return Value::operator^( other );
}

dawn::RawValue dawn::IntValue::operator%( Value const& other ) const
{
    if ( other.type() == tp_int )
        return make_int_value( value % other.to_int() );

    if ( other.type() == tp_float )
        return make_float_value( mymod( (Float) value, other.to_float() ) );

    return Value::operator%( other );
}

dawn::Int dawn::IntValue::operator<=>( Value const& other ) const
{
    if ( other.type() == tp_int )
    {
        auto result = value <=> other.to_int();
        return result._Value;
    }

    if ( other.type() == tp_float )
    {
        auto result = to_float() <=> other.to_float();
        return result._Value;
    }

    return Value::operator<=>( other );
}

dawn::RawValue dawn::IntValue::operator>>( Value const& other ) const
{
    auto result = std::make_shared<RangeValue>();
    result->start_incl = value;
    result->end_excl = other.to_int();
    return result;
}

dawn::Bool dawn::IntValue::to_bool() const
{
    return (Bool) value;
}

dawn::Int dawn::IntValue::to_int() const
{
    return value;
}

dawn::Float dawn::IntValue::to_float() const
{
    return (Float) value;
}

dawn::Char dawn::IntValue::to_char() const
{
    return (Char) value;
}

dawn::String dawn::IntValue::to_string() const
{
    return std::to_wstring( value );
}

// float
dawn::StringRef const& dawn::FloatValue::type() const
{
    return tp_float;
}

dawn::RawValue dawn::FloatValue::clone() const
{
    return make_float_value( value );
}

dawn::RawValue dawn::FloatValue::operator-() const
{
    return make_float_value( -value );
}

dawn::RawValue dawn::FloatValue::operator+( Value const& other ) const
{
    if ( other.type() == tp_int )
        return make_float_value( value + other.to_int() );

    if ( other.type() == tp_float )
        return make_float_value( value + other.to_float() );

    return Value::operator+( other );
}

dawn::RawValue dawn::FloatValue::operator-( Value const& other ) const
{
    if ( other.type() == tp_int )
        return make_float_value( value - other.to_int() );

    if ( other.type() == tp_float )
        return make_float_value( value - other.to_float() );

    return Value::operator-( other );
}

dawn::RawValue dawn::FloatValue::operator*( Value const& other ) const
{
    if ( other.type() == tp_int )
        return make_float_value( value * other.to_int() );

    if ( other.type() == tp_float )
        return make_float_value( value * other.to_float() );

    return Value::operator*( other );
}

dawn::RawValue dawn::FloatValue::operator/( Value const& other ) const
{
    if ( other.type() == tp_int )
        return make_float_value( value / other.to_int() );

    if ( other.type() == tp_float )
        return make_float_value( value / other.to_float() );

    return Value::operator/( other );
}

dawn::RawValue dawn::FloatValue::operator^( Value const& other ) const
{
    if ( other.type() == tp_int )
        return make_float_value( std::pow( value, other.to_int() ) );

    if ( other.type() == tp_float )
        return make_float_value( std::pow( value, other.to_float() ) );

    return Value::operator^( other );
}

dawn::RawValue dawn::FloatValue::operator%( Value const& other ) const
{
    if ( other.type() == tp_int )
        return make_float_value( mymod( value, (Float) other.to_int() ) );

    if ( other.type() == tp_float )
        return make_float_value( mymod( value, other.to_float() ) );

    return Value::operator%( other );
}

dawn::Int dawn::FloatValue::operator<=>( Value const& other ) const
{
    if ( other.type() == tp_float )
    {
        auto result = value <=> other.to_float();
        return result._Value;
    }

    if ( other.type() == tp_int )
    {
        auto result = value <=> other.to_float();
        return result._Value;
    }

    return Value::operator<=>( other );
}

dawn::Bool dawn::FloatValue::to_bool() const
{
    return (Bool) value;
}

dawn::Int dawn::FloatValue::to_int() const
{
    return (Int) value;
}

dawn::Float dawn::FloatValue::to_float() const
{
    return value;
}

dawn::Char dawn::FloatValue::to_char() const
{
    return (Char) value;
}

dawn::String dawn::FloatValue::to_string() const
{
    String result = format( value );
    if ( std::to_wstring( Int( value ) ) == result )
        result += L".0";
    return result;
}

// char
dawn::StringRef const& dawn::CharValue::type() const
{
    return tp_char;
}

dawn::RawValue dawn::CharValue::clone() const
{
    return make_char_value( value );
}

dawn::Int dawn::CharValue::operator<=>( Value const& other ) const
{
    if ( other.type() == tp_char )
    {
        auto result = value <=> other.to_char();
        return result._Value;
    }

    return Value::operator<=>( other );
}

dawn::Bool dawn::CharValue::to_bool() const
{
    return value == L't';
}

dawn::Int dawn::CharValue::to_int() const
{
    return (Int) value;
}

dawn::Float dawn::CharValue::to_float() const
{
    return (Float) value;
}

dawn::Char dawn::CharValue::to_char() const
{
    return value;
}

dawn::String dawn::CharValue::to_string() const
{
    return String( 1, value );
}

// string
dawn::StringRef const& dawn::StringValue::type() const
{
    return tp_string;
}

dawn::RawValue dawn::StringValue::clone() const
{
    return make_string_value( value );
}

dawn::RawValue dawn::StringValue::operator+( Value const& other ) const
{
    if ( other.type() == tp_string )
        return make_string_value( value + other.to_string() );

    return Value::operator+( other );
}

dawn::Int dawn::StringValue::operator<=>( Value const& other ) const
{
    if ( other.type() == tp_string )
    {
        auto result = value <=> other.to_string();
        return result._Value;
    }

    return Value::operator<=>( other );
}

dawn::Bool dawn::StringValue::to_bool() const
{
    return value == kw_true;
}

dawn::Int dawn::StringValue::to_int() const
{
    auto optres = parse_int( value );
    if ( !optres )
        PANIC( "string \"", value, "\" to int failed" );
    return *optres;
}

dawn::Float dawn::StringValue::to_float() const
{
    auto optres = parse_float( value );
    if ( !optres )
        PANIC( "string \"", value, "\" to float failed" );
    return *optres;
}

dawn::Char dawn::StringValue::to_char() const
{
    return value.front();
}

dawn::String dawn::StringValue::to_string() const
{
    return value;
}

// enum
dawn::StringRef const& dawn::EnumValue::type() const
{
    return tp_enum;
}

dawn::RawValue dawn::EnumValue::clone() const
{
    auto result = std::make_shared<EnumValue>();
    result->parent = parent;
    result->key = key;
    return result;
}

dawn::String dawn::EnumValue::to_string() const
{
    return parent->name + L":" + key;
}

// struct
dawn::StringRef const& dawn::StructValue::type() const
{
    return tp_struct;
}

dawn::RawValue dawn::StructValue::clone() const
{
    auto result = std::make_shared<StructValue>();
    result->parent = parent;
    for ( auto& [key, value] : members )
        result->members[key] = ValueBox{ ValueKind::LET, value.value() };
    return result;
}

dawn::String dawn::StructValue::to_string() const
{
    if ( members.empty() )
        return parent->name + L"{}";

    StringStream stream;
    stream << parent->name << L"{ ";
    for ( auto it = members.begin(); it != members.end(); it++ )
    {
        auto& [key, value] = *it;
        stream << key << L": " << value.value()->to_string();
        Map<String, ValueBox>::const_iterator next_it = it;
        ++next_it;
        stream << (next_it == members.end() ? L" }" : L", ");
    }
    return stream.str();
}

// array
dawn::StringRef const& dawn::ArrayValue::type() const
{
    return tp_array;
}

dawn::RawValue dawn::ArrayValue::clone() const
{
    auto result = std::make_shared<ArrayValue>();
    result->data.reserve( data.size() );
    for ( auto& value : data )
        result->data.emplace_back( ValueKind::LET, value.value() );
    return result;
}

dawn::RawValue dawn::ArrayValue::operator+( Value const& other ) const
{
    if ( auto other_arr = dynamic_cast<ArrayValue const*>(&other) )
    {
        auto result = std::make_shared<ArrayValue>();
        result->data.reserve( data.size() + other_arr->data.size() );
        for ( auto& value : data )
            result->data.emplace_back( ValueKind::LET, value.value() );
        for ( auto& value : other_arr->data )
            result->data.emplace_back( ValueKind::LET, value.value() );
        return result;
    }

    return Value::operator+( other );
}

dawn::Int dawn::ArrayValue::operator<=>( Value const& other ) const
{
    return Value::operator<=>( other );
}

dawn::String dawn::ArrayValue::to_string() const
{
    if ( data.empty() )
        return L"[]";

    StringStream stream;
    stream << L"[";
    for ( Int i = 0; i < (Int) data.size() - 1; i++ )
        stream << data[i].value()->to_string() << L", ";
    stream << data.back().value()->to_string() << L"]";

    return stream.str();
}

dawn::StringRef const& dawn::RangeValue::type() const
{
    return tp_range;
}

dawn::RawValue dawn::RangeValue::clone() const
{
    auto result = std::make_shared<RangeValue>();
    result->start_incl = start_incl;
    result->end_excl = end_excl;
    return result;
}

dawn::String dawn::RangeValue::to_string() const
{
    return format( start_incl, L"~", end_excl );
}

// helpers
dawn::RawValue dawn::make_nothing_value()
{
    return std::make_shared<NothingValue>();
}

dawn::RawValue dawn::make_bool_value( Bool value )
{
    auto result = std::make_shared<BoolValue>();
    result->value = value;
    return result;
}

dawn::RawValue dawn::make_int_value( Int value )
{
    auto result = std::make_shared<IntValue>();
    result->value = value;
    return result;
}

dawn::RawValue dawn::make_float_value( Float value )
{
    auto result = std::make_shared<FloatValue>();
    result->value = value;
    return result;
}

dawn::RawValue dawn::make_char_value( Char value )
{
    auto result = std::make_shared<CharValue>();
    result->value = value;
    return result;
}

dawn::RawValue dawn::make_string_value( StringRef const& value )
{
    auto result = std::make_shared<StringValue>();
    result->value = value;
    return result;
}
