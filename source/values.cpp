#include "values.h"
#include "engine.h"


// ref
dawn::StringRef const& dawn::RefValue::type() const
{
    return eng_var->get_value()->type();
}

dawn::Ref<dawn::Value> dawn::RefValue::clone() const
{
    return eng_var->get_value()->clone();
}

dawn::Ref<dawn::Value> dawn::RefValue::operator-() const
{
    return eng_var->get_value()->operator-();
}

dawn::Ref<dawn::Value> dawn::RefValue::operator+( Value const& other ) const
{
    return eng_var->get_value()->operator+( other );
}

dawn::Ref<dawn::Value> dawn::RefValue::operator-( Value const& other ) const
{
    return eng_var->get_value()->operator-( other );
}

dawn::Ref<dawn::Value> dawn::RefValue::operator*( Value const& other ) const
{
    return eng_var->get_value()->operator*( other );
}

dawn::Ref<dawn::Value> dawn::RefValue::operator/( Value const& other ) const
{
    return eng_var->get_value()->operator/( other );
}

dawn::Ref<dawn::Value> dawn::RefValue::operator^( Value const& other ) const
{
    return eng_var->get_value()->operator^( other );
}

dawn::Ref<dawn::Value> dawn::RefValue::operator%( Value const& other ) const
{
    return eng_var->get_value()->operator%( other );
}

dawn::Int dawn::RefValue::operator<=>( Value const& other ) const
{
    return eng_var->get_value()->operator<=>( other );
}

dawn::Ref<dawn::Value> dawn::RefValue::operator>>( Value const& other ) const
{
    return eng_var->get_value()->operator>>( other );
}

dawn::Bool dawn::RefValue::to_bool() const
{
    return eng_var->get_value()->to_bool();
}

dawn::Int dawn::RefValue::to_int() const
{
    return eng_var->get_value()->to_int();
}

dawn::Float dawn::RefValue::to_float() const
{
    return eng_var->get_value()->to_float();
}

dawn::Char dawn::RefValue::to_char() const
{
    return eng_var->get_value()->to_char();
}

dawn::String dawn::RefValue::to_string() const
{
    return eng_var->get_value()->to_string();
}

// nothing
dawn::StringRef const& dawn::NothingValue::type() const
{
    static constexpr StringRef tp_nothing = L"nothing";
    return tp_nothing;
}

dawn::Ref<dawn::Value> dawn::NothingValue::clone() const
{
    return NothingValue::make();
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

dawn::Ref<dawn::Value> dawn::BoolValue::clone() const
{
    auto result = BoolValue::make();
    result->value = value;
    return result;
}

dawn::Int dawn::BoolValue::operator<=>( Value const& other ) const
{
    auto result = value <=> other.to_bool();
    return result._Value;
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
        return L'T';
    return L'F';
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

dawn::Ref<dawn::Value> dawn::IntValue::clone() const
{
    auto result = IntValue::make();
    result->value = value;
    return result;
}

dawn::Ref<dawn::Value> dawn::IntValue::operator-() const
{
    auto result = IntValue::make();
    result->value = -value;
    return result;
}

dawn::Ref<dawn::Value> dawn::IntValue::operator+( Value const& other ) const
{
    if ( other.type() == tp_int )
    {
        auto result = IntValue::make();
        result->value = value + other.to_int();
        return result;
    }

    if ( other.type() == tp_float )
    {
        auto result = FloatValue::make();
        result->value = value + other.to_float();
        return result;
    }

    return Value::operator+( other );
}

dawn::Ref<dawn::Value> dawn::IntValue::operator-( Value const& other ) const
{
    if ( other.type() == tp_int )
    {
        auto result = IntValue::make();
        result->value = value - other.to_int();
        return result;
    }

    if ( other.type() == tp_float )
    {
        auto result = FloatValue::make();
        result->value = value - other.to_float();
        return result;
    }

    return Value::operator-( other );
}

dawn::Ref<dawn::Value> dawn::IntValue::operator*( Value const& other ) const
{
    if ( other.type() == tp_int )
    {
        auto result = IntValue::make();
        result->value = value * other.to_int();
        return result;
    }

    if ( other.type() == tp_float )
    {
        auto result = FloatValue::make();
        result->value = value * other.to_float();
        return result;
    }

    return Value::operator*( other );
}

dawn::Ref<dawn::Value> dawn::IntValue::operator/( Value const& other ) const
{
    if ( other.type() == tp_int )
    {
        auto result = IntValue::make();
        result->value = value / other.to_int();
        return result;
    }

    if ( other.type() == tp_float )
    {
        auto result = FloatValue::make();
        result->value = value / other.to_float();
        return result;
    }

    return Value::operator/( other );
}

dawn::Ref<dawn::Value> dawn::IntValue::operator^( Value const& other ) const
{
    if ( other.type() == tp_int )
    {
        auto result = IntValue::make();
        result->value = (Int) std::pow( value, other.to_int() );
        return result;
    }

    if ( other.type() == tp_float )
    {
        auto result = FloatValue::make();
        result->value = (Float) std::pow( value, other.to_float() );
        return result;
    }

    return Value::operator^( other );
}

dawn::Ref<dawn::Value> dawn::IntValue::operator%( Value const& other ) const
{
    if ( other.type() == tp_int )
    {
        auto result = IntValue::make();
        result->value = value % other.to_int();
        return result;
    }

    if ( other.type() == tp_float )
    {
        auto result = FloatValue::make();
        result->value = mymod( (Float) value, other.to_float() );
        return result;
    }

    return Value::operator%( other );
}

dawn::Int dawn::IntValue::operator<=>( Value const& other ) const
{
    if ( other.type() == tp_float )
    {
        auto result = to_float() <=> other.to_float();
        return result._Value;
    }

    auto result = value <=> other.to_int();
    return result._Value;
}

dawn::Ref<dawn::Value> dawn::IntValue::operator>>( Value const& other ) const
{
    auto result = RangeValue::make();
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

dawn::Ref<dawn::Value> dawn::FloatValue::clone() const
{
    auto result = FloatValue::make();
    result->value = value;
    return result;
}

dawn::Ref<dawn::Value> dawn::FloatValue::operator-() const
{
    auto result = FloatValue::make();
    result->value = -value;
    return result;
}

dawn::Ref<dawn::Value> dawn::FloatValue::operator+( Value const& other ) const
{
    if ( other.type() == tp_int )
    {
        auto result = FloatValue::make();
        result->value = value + other.to_int();
        return result;
    }

    if ( other.type() == tp_float )
    {
        auto result = FloatValue::make();
        result->value = value + other.to_float();
        return result;
    }

    return Value::operator+( other );
}

dawn::Ref<dawn::Value> dawn::FloatValue::operator-( Value const& other ) const
{
    if ( other.type() == tp_int )
    {
        auto result = FloatValue::make();
        result->value = value - other.to_int();
        return result;
    }

    if ( other.type() == tp_float )
    {
        auto result = FloatValue::make();
        result->value = value - other.to_float();
        return result;
    }

    return Value::operator-( other );
}

dawn::Ref<dawn::Value> dawn::FloatValue::operator*( Value const& other ) const
{
    if ( other.type() == tp_int )
    {
        auto result = FloatValue::make();
        result->value = value * other.to_int();
        return result;
    }

    if ( other.type() == tp_float )
    {
        auto result = FloatValue::make();
        result->value = value * other.to_float();
        return result;
    }

    return Value::operator*( other );
}

dawn::Ref<dawn::Value> dawn::FloatValue::operator/( Value const& other ) const
{
    if ( other.type() == tp_int )
    {
        auto result = FloatValue::make();
        result->value = value / other.to_int();
        return result;
    }

    if ( other.type() == tp_float )
    {
        auto result = FloatValue::make();
        result->value = value / other.to_float();
        return result;
    }

    return Value::operator/( other );
}

dawn::Ref<dawn::Value> dawn::FloatValue::operator^( Value const& other ) const
{
    if ( other.type() == tp_int )
    {
        auto result = FloatValue::make();
        result->value = std::pow( value, other.to_int() );
        return result;
    }

    if ( other.type() == tp_float )
    {
        auto result = FloatValue::make();
        result->value = std::pow( value, other.to_float() );
        return result;
    }

    return Value::operator^( other );
}

dawn::Ref<dawn::Value> dawn::FloatValue::operator%( Value const& other ) const
{
    if ( other.type() == tp_int )
    {
        auto result = FloatValue::make();
        result->value = mymod( value, (Float) other.to_int() );
        return result;
    }

    if ( other.type() == tp_float )
    {
        auto result = FloatValue::make();
        result->value = mymod( value, other.to_float() );
        return result;
    }

    return Value::operator%( other );
}

dawn::Int dawn::FloatValue::operator<=>( Value const& other ) const
{
    auto result = value <=> other.to_float();
    return result._Value;
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
    return std::to_wstring( value );
}

// char
dawn::StringRef const& dawn::CharValue::type() const
{
    return tp_char;
}

dawn::Ref<dawn::Value> dawn::CharValue::clone() const
{
    auto result = CharValue::make();
    result->value = value;
    return result;
}

dawn::Int dawn::CharValue::operator<=>( Value const& other ) const
{
    auto result = value <=> other.to_char();
    return result._Value;
}

dawn::Bool dawn::CharValue::to_bool() const
{
    return value == L'T';
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

dawn::Ref<dawn::Value> dawn::StringValue::clone() const
{
    auto result = StringValue::make();
    result->value = value;
    return result;
}

dawn::Ref<dawn::Value> dawn::StringValue::operator+( Value const& other ) const
{
    if ( other.type() == tp_string )
    {
        auto result = StringValue::make();
        result->value = value + other.to_string();
        return result;
    }

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
    static constexpr StringRef tp_enum = L"enum";
    return tp_enum;
}

dawn::Ref<dawn::Value> dawn::EnumValue::clone() const
{
    auto result = EnumValue::make();
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
    static constexpr StringRef tp_struct = L"struct";
    return tp_struct;
}

dawn::Ref<dawn::Value> dawn::StructValue::clone() const
{
    auto result = StructValue::make();
    result->parent = parent;
    for ( auto& [key, value] : members )
        result->members[key] = value->clone();
    return result;
}

dawn::String dawn::StructValue::to_string() const
{
    return parent->name + L"{}";
}

// array
dawn::StringRef const& dawn::ArrayValue::type() const
{
    static constexpr StringRef tp_array = L"array";
    return tp_array;
}

dawn::Ref<dawn::Value> dawn::ArrayValue::clone() const
{
    auto result = ArrayValue::make();
    result->data.reserve( data.size() );
    for ( auto& value : data )
        result->data.push_back( value->clone() );
    return result;
}

dawn::Ref<dawn::Value> dawn::ArrayValue::operator+( Value const& other ) const
{
    if ( auto other_arr = dynamic_cast<ArrayValue const*>(&other) )
    {
        auto result = ArrayValue::make();
        result->data.reserve( data.size() + other_arr->data.size() );
        for ( auto& value : data )
            result->data.push_back( value->clone() );
        for ( auto& value : other_arr->data )
            result->data.push_back( value->clone() );
        return result;
    }

    return Value::operator+( other );
}

dawn::Int dawn::ArrayValue::operator<=>( Value const& other ) const
{
    if ( auto other_arr = dynamic_cast<ArrayValue const*>(&other) )
    {
        auto result = data <=> other_arr->data;
        return result._Value;
    }

    return Value::operator<=>( other );
}

dawn::String dawn::ArrayValue::to_string() const
{
    if ( data.empty() )
        return L"[]";

    StringStream stream;
    stream << L"[";
    for ( Int i = 0; i < (Int) data.size() - 1; i++ )
        stream << data[i]->to_string() << L", ";
    stream << data.back()->to_string() << L"]";

    return stream.str();
}

dawn::StringRef const& dawn::RangeValue::type() const
{
    static constexpr StringRef tp_range = L"range";
    return tp_range;
}

dawn::Ref<dawn::Value> dawn::RangeValue::clone() const
{
    auto result = RangeValue::make();
    result->start_incl = start_incl;
    result->end_excl = end_excl;
    return result;
}

dawn::String dawn::RangeValue::to_string() const
{
    return format( start_incl, L"~", end_excl );
}
