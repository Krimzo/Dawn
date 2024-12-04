#include "values.h"


// bool
dawn::Ref<dawn::Value> dawn::BoolValue::operator+() const
{
    return _chaos( "+bool not supported" );
}

dawn::Ref<dawn::Value> dawn::BoolValue::operator-() const
{
    return _chaos( "-bool not supported" );
}

dawn::Ref<dawn::Value> dawn::BoolValue::operator+( Value const& other ) const
{
    return _chaos( "bool + not supported" );
}

dawn::Ref<dawn::Value> dawn::BoolValue::operator-( Value const& other ) const
{
    return _chaos( "bool - not supported" );
}

dawn::Ref<dawn::Value> dawn::BoolValue::operator*( Value const& other ) const
{
    return _chaos( "bool * not supported" );
}

dawn::Ref<dawn::Value> dawn::BoolValue::operator/( Value const& other ) const
{
    return _chaos( "bool / not supported" );
}

dawn::Ref<dawn::Value> dawn::BoolValue::operator^( Value const& other ) const
{
    return _chaos( "bool ^ not supported" );
}

dawn::Ref<dawn::Value> dawn::BoolValue::operator%( Value const& other ) const
{
    return _chaos( "bool % not supported" );
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

dawn::Ref<dawn::Value> dawn::BoolValue::clone() const
{
    auto result = std::make_shared<BoolValue>();
    result->value = value;
    return result;
}

// int
dawn::Ref<dawn::Value> dawn::IntValue::operator+() const
{
    auto result = std::dynamic_pointer_cast<IntValue>(clone());
    result->value = +value;
    return result;
}

dawn::Ref<dawn::Value> dawn::IntValue::operator-() const
{
    auto result = std::dynamic_pointer_cast<IntValue>(clone());
    result->value = -value;
    return result;
}

dawn::Ref<dawn::Value> dawn::IntValue::operator+( Value const& other ) const
{
    if ( auto other_int = dynamic_cast<IntValue const*>(&other) )
    {
        auto result = std::make_shared<IntValue>();
        result->value = value + other_int->value;
        return result;
    }

    if ( auto other_flt = dynamic_cast<FloatValue const*>(&other) )
    {
        auto result = std::make_shared<FloatValue>();
        result->value = value + other_flt->value;
        return result;
    }

    return _chaos( "int + ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::IntValue::operator-( Value const& other ) const
{
    if ( auto other_int = dynamic_cast<IntValue const*>(&other) )
    {
        auto result = std::make_shared<IntValue>();
        result->value = value - other_int->value;
        return result;
    }

    if ( auto other_flt = dynamic_cast<FloatValue const*>(&other) )
    {
        auto result = std::make_shared<FloatValue>();
        result->value = value - other_flt->value;
        return result;
    }

    return _chaos( "int - ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::IntValue::operator*( Value const& other ) const
{
    if ( auto other_int = dynamic_cast<IntValue const*>(&other) )
    {
        auto result = std::make_shared<IntValue>();
        result->value = value * other_int->value;
        return result;
    }

    if ( auto other_flt = dynamic_cast<FloatValue const*>(&other) )
    {
        auto result = std::make_shared<FloatValue>();
        result->value = value * other_flt->value;
        return result;
    }

    return _chaos( "int * ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::IntValue::operator/( Value const& other ) const
{
    if ( auto other_int = dynamic_cast<IntValue const*>(&other) )
    {
        auto result = std::make_shared<IntValue>();
        result->value = value / other_int->value;
        return result;
    }

    if ( auto other_flt = dynamic_cast<FloatValue const*>(&other) )
    {
        auto result = std::make_shared<FloatValue>();
        result->value = value / other_flt->value;
        return result;
    }

    return _chaos( "int / ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::IntValue::operator^( Value const& other ) const
{
    if ( auto other_int = dynamic_cast<IntValue const*>(&other) )
    {
        auto result = std::make_shared<IntValue>();
        result->value = (Int) std::pow( value, other_int->value );
        return result;
    }

    if ( auto other_flt = dynamic_cast<FloatValue const*>(&other) )
    {
        auto result = std::make_shared<FloatValue>();
        result->value = (Float) std::pow( value, other_flt->value );
        return result;
    }

    return _chaos( "int ^ ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::IntValue::operator%( Value const& other ) const
{
    if ( auto other_int = dynamic_cast<IntValue const*>(&other) )
    {
        auto result = std::make_shared<IntValue>();
        result->value = value % other_int->value;
        return result;
    }

    if ( auto other_flt = dynamic_cast<FloatValue const*>(&other) )
    {
        auto result = std::make_shared<FloatValue>();
        result->value = std::remainder( value, other_flt->value );
        return result;
    }

    return _chaos( "int ^ ", typeid(other).name(), " not supported" );
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

dawn::Ref<dawn::Value> dawn::IntValue::clone() const
{
    auto result = std::make_shared<IntValue>();
    result->value = value;
    return result;
}

// float
dawn::Ref<dawn::Value> dawn::FloatValue::operator+() const
{
    auto result = std::dynamic_pointer_cast<FloatValue>(clone());
    result->value = +value;
    return result;
}

dawn::Ref<dawn::Value> dawn::FloatValue::operator-() const
{
    auto result = std::dynamic_pointer_cast<FloatValue>(clone());
    result->value = -value;
    return result;
}

dawn::Ref<dawn::Value> dawn::FloatValue::operator+( Value const& other ) const
{
    if ( auto other_int = dynamic_cast<IntValue const*>(&other) )
    {
        auto result = std::make_shared<FloatValue>();
        result->value = value + other_int->value;
        return result;
    }

    if ( auto other_flt = dynamic_cast<FloatValue const*>(&other) )
    {
        auto result = std::make_shared<FloatValue>();
        result->value = value + other_flt->value;
        return result;
    }

    return _chaos( "float + ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::FloatValue::operator-( Value const& other ) const
{
    if ( auto other_int = dynamic_cast<IntValue const*>(&other) )
    {
        auto result = std::make_shared<FloatValue>();
        result->value = value - other_int->value;
        return result;
    }

    if ( auto other_flt = dynamic_cast<FloatValue const*>(&other) )
    {
        auto result = std::make_shared<FloatValue>();
        result->value = value - other_flt->value;
        return result;
    }

    return _chaos( "float - ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::FloatValue::operator*( Value const& other ) const
{
    if ( auto other_int = dynamic_cast<IntValue const*>(&other) )
    {
        auto result = std::make_shared<FloatValue>();
        result->value = value * other_int->value;
        return result;
    }

    if ( auto other_flt = dynamic_cast<FloatValue const*>(&other) )
    {
        auto result = std::make_shared<FloatValue>();
        result->value = value * other_flt->value;
        return result;
    }

    return _chaos( "float * ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::FloatValue::operator/( Value const& other ) const
{
    if ( auto other_int = dynamic_cast<IntValue const*>(&other) )
    {
        auto result = std::make_shared<FloatValue>();
        result->value = value / other_int->value;
        return result;
    }

    if ( auto other_flt = dynamic_cast<FloatValue const*>(&other) )
    {
        auto result = std::make_shared<FloatValue>();
        result->value = value / other_flt->value;
        return result;
    }

    return _chaos( "float / ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::FloatValue::operator^( Value const& other ) const
{
    if ( auto other_int = dynamic_cast<IntValue const*>(&other) )
    {
        auto result = std::make_shared<FloatValue>();
        result->value = std::pow( value, other_int->value );
        return result;
    }

    if ( auto other_flt = dynamic_cast<FloatValue const*>(&other) )
    {
        auto result = std::make_shared<FloatValue>();
        result->value = std::pow( value, other_flt->value );
        return result;
    }

    return _chaos( "float ^ ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::FloatValue::operator%( Value const& other ) const
{
    if ( auto other_int = dynamic_cast<IntValue const*>(&other) )
    {
        auto result = std::make_shared<FloatValue>();
        result->value = std::remainder( value, other_int->value );
        return result;
    }

    if ( auto other_flt = dynamic_cast<FloatValue const*>(&other) )
    {
        auto result = std::make_shared<FloatValue>();
        result->value = std::remainder( value, other_flt->value );
        return result;
    }

    return _chaos( "float % ", typeid(other).name(), " not supported" );
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

dawn::Ref<dawn::Value> dawn::FloatValue::clone() const
{
    auto result = std::make_shared<FloatValue>();
    result->value = value;
    return result;
}

// char
dawn::Ref<dawn::Value> dawn::CharValue::operator+() const
{
    return _chaos( "+char not supported" );
}

dawn::Ref<dawn::Value> dawn::CharValue::operator-() const
{
    return _chaos( "-char not supported" );
}

dawn::Ref<dawn::Value> dawn::CharValue::operator+( Value const& other ) const
{
    return _chaos( "char + ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::CharValue::operator-( Value const& other ) const
{
    return _chaos( "char - ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::CharValue::operator*( Value const& other ) const
{
    return _chaos( "char * ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::CharValue::operator/( Value const& other ) const
{
    return _chaos( "char / ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::CharValue::operator^( Value const& other ) const
{
    return _chaos( "char ^ ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::CharValue::operator%( Value const& other ) const
{
    return _chaos( "char % ", typeid(other).name(), " not supported" );
}

dawn::Bool dawn::CharValue::to_bool() const
{
    return (Bool) value;
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

dawn::Ref<dawn::Value> dawn::CharValue::clone() const
{
    auto result = std::make_shared<CharValue>();
    result->value = value;
    return result;
}

// string
dawn::Ref<dawn::Value> dawn::StringValue::operator+() const
{
    return _chaos( "+string not supported" );
}

dawn::Ref<dawn::Value> dawn::StringValue::operator-() const
{
    return _chaos( "-string not supported" );
}

dawn::Ref<dawn::Value> dawn::StringValue::operator+( Value const& other ) const
{
    if ( auto other_str = dynamic_cast<StringValue const*>(&other) )
    {
        auto result = std::make_shared<StringValue>();
        result->value = value + other_str->value;
        return result;
    }

    return _chaos( "string + ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::StringValue::operator-( Value const& other ) const
{
    return _chaos( "string - ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::StringValue::operator*( Value const& other ) const
{
    return _chaos( "string * ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::StringValue::operator/( Value const& other ) const
{
    return _chaos( "string / ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::StringValue::operator^( Value const& other ) const
{
    return _chaos( "string ^ ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::StringValue::operator%( Value const& other ) const
{
    return _chaos( "string % ", typeid(other).name(), " not supported" );
}

dawn::Bool dawn::StringValue::to_bool() const
{
    return value == kw_true;
}

dawn::Int dawn::StringValue::to_int() const
{
    return std::stoll( value );
}

dawn::Float dawn::StringValue::to_float() const
{
    return std::stod( value );
}

dawn::Char dawn::StringValue::to_char() const
{
    return value.front();
}

dawn::String dawn::StringValue::to_string() const
{
    return value;
}

dawn::Ref<dawn::Value> dawn::StringValue::clone() const
{
    auto result = std::make_shared<StringValue>();
    result->value = value;
    return result;
}

// enum
dawn::Ref<dawn::Value> dawn::EnumValue::operator+() const
{
    return _chaos( "+enum not supported" );
}

dawn::Ref<dawn::Value> dawn::EnumValue::operator-() const
{
    return _chaos( "-enum not supported" );
}

dawn::Ref<dawn::Value> dawn::EnumValue::operator+( Value const& other ) const
{
    return _chaos( "enum + ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::EnumValue::operator-( Value const& other ) const
{
    return _chaos( "enum - ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::EnumValue::operator*( Value const& other ) const
{
    return _chaos( "enum * ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::EnumValue::operator/( Value const& other ) const
{
    return _chaos( "enum / ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::EnumValue::operator^( Value const& other ) const
{
    return _chaos( "enum ^ ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::EnumValue::operator%( Value const& other ) const
{
    return _chaos( "enum % ", typeid(other).name(), " not supported" );
}

dawn::Bool dawn::EnumValue::to_bool() const
{
    _chaos( "enum to bool not supported" );
    return {};
}

dawn::Int dawn::EnumValue::to_int() const
{
    _chaos( "enum to int not supported" );
    return {};
}

dawn::Float dawn::EnumValue::to_float() const
{
    _chaos( "enum to float not supported" );
    return {};
}

dawn::Char dawn::EnumValue::to_char() const
{
    _chaos( "enum to char not supported" );
    return {};
}

dawn::String dawn::EnumValue::to_string() const
{
    return parent->name + L":" + key;
}

dawn::Ref<dawn::Value> dawn::EnumValue::clone() const
{
    auto result = std::make_shared<EnumValue>();
    result->parent = parent;
    result->key = key;
    return result;
}

// struct
dawn::Ref<dawn::Value> dawn::StructValue::operator+() const
{
    return _chaos( "+struct not supported" );
}

dawn::Ref<dawn::Value> dawn::StructValue::operator-() const
{
    return _chaos( "-struct not supported" );
}

dawn::Ref<dawn::Value> dawn::StructValue::operator+( Value const& other ) const
{
    return _chaos( "struct + ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::StructValue::operator-( Value const& other ) const
{
    return _chaos( "struct - ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::StructValue::operator*( Value const& other ) const
{
    return _chaos( "struct * ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::StructValue::operator/( Value const& other ) const
{
    return _chaos( "struct / ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::StructValue::operator^( Value const& other ) const
{
    return _chaos( "struct ^ ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::StructValue::operator%( Value const& other ) const
{
    _chaos( "struct % ", typeid(other).name(), " not supported" );
    return {};
}

dawn::Bool dawn::StructValue::to_bool() const
{
    _chaos( "struct to bool not supported" );
    return {};
}

dawn::Int dawn::StructValue::to_int() const
{
    _chaos( "struct to int not supported" );
    return {};
}

dawn::Float dawn::StructValue::to_float() const
{
    _chaos( "struct to float not supported" );
    return {};
}

dawn::Char dawn::StructValue::to_char() const
{
    _chaos( "struct to char not supported" );
    return {};
}

dawn::String dawn::StructValue::to_string() const
{
    return parent->name + L"{}";
}

dawn::Ref<dawn::Value> dawn::StructValue::clone() const
{
    auto result = std::make_shared<StructValue>();
    result->parent = parent;
    for ( auto& [key, value] : members )
        result->members[key] = value->clone();
    return result;
}

// array
dawn::Ref<dawn::Value> dawn::ArrayValue::operator+() const
{
    return _chaos( "+array not supported" );
}

dawn::Ref<dawn::Value> dawn::ArrayValue::operator-() const
{
    return _chaos( "-array not supported" );
}

dawn::Ref<dawn::Value> dawn::ArrayValue::operator+( Value const& other ) const
{
    if ( auto other_arr = dynamic_cast<ArrayValue const*>(&other) )
    {
        auto result = std::make_shared<ArrayValue>();
        result->data.reserve( data.size() + other_arr->data.size() );
        for ( auto& value : data )
            result->data.push_back( value->clone() );
        for ( auto& value : other_arr->data )
            result->data.push_back( value->clone() );
        return result;
    }

    return _chaos( "array + ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::ArrayValue::operator-( Value const& other ) const
{
    return _chaos( "array - ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::ArrayValue::operator*( Value const& other ) const
{
    return _chaos( "array * ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::ArrayValue::operator/( Value const& other ) const
{
    return _chaos( "array / ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::ArrayValue::operator^( Value const& other ) const
{
    return _chaos( "array ^ ", typeid(other).name(), " not supported" );
}

dawn::Ref<dawn::Value> dawn::ArrayValue::operator%( Value const& other ) const
{
    return _chaos( "array % ", typeid(other).name(), " not supported" );
}

dawn::Bool dawn::ArrayValue::to_bool() const
{
    _chaos( "array to bool not supported" );
    return {};
}

dawn::Int dawn::ArrayValue::to_int() const
{
    _chaos( "array to int not supported" );
    return {};
}

dawn::Float dawn::ArrayValue::to_float() const
{
    _chaos( "array to float not supported" );
    return {};
}

dawn::Char dawn::ArrayValue::to_char() const
{
    _chaos( "array to char not supported" );
    return {};
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

dawn::Ref<dawn::Value> dawn::ArrayValue::clone() const
{
    auto result = std::make_shared<ArrayValue>();
    result->data.reserve( data.size() );
    for ( auto& value : data )
        result->data.push_back( value->clone() );
    return result;
}
