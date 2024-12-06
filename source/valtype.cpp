#include "valtype.h"
#include "values.h"


dawn::Ref<dawn::Value> dawn::Type::construct() const
{
    PANIC( "type [", name, "] can't be constructed" );
}

dawn::Ref<dawn::Value> dawn::Value::operator-() const
{
    PANIC( "unary - for [", type(), "] not supported" );
}

dawn::Ref<dawn::Value> dawn::Value::operator+( Value const& other ) const
{
    PANIC( "[", type(), "] + [", other.type(), "] not supported" );
}

dawn::Ref<dawn::Value> dawn::Value::operator-( Value const& other ) const
{
    PANIC( "[", type(), "] - [", other.type(), "] not supported" );
}

dawn::Ref<dawn::Value> dawn::Value::operator*( Value const& other ) const
{
    PANIC( "[", type(), "] * [", other.type(), "] not supported" );
}

dawn::Ref<dawn::Value> dawn::Value::operator/( Value const& other ) const
{
    PANIC( "[", type(), "] / [", other.type(), "] not supported" );
}

dawn::Ref<dawn::Value> dawn::Value::operator^( Value const& other ) const
{
    PANIC( "[", type(), "] ^ [", other.type(), "] not supported" );
}

dawn::Ref<dawn::Value> dawn::Value::operator%( Value const& other ) const
{
    PANIC( "[", type(), "] % [", other.type(), "] not supported" );
}

dawn::Int dawn::Value::operator<=>( Value const& other ) const
{
    PANIC( "[", type(), "] <=> [", other.type(), "] not supported" );
}

dawn::Ref<dawn::Value> dawn::Value::operator==( Value const& other ) const
{
    auto result = BoolValue::make();
    result->value = ((*this <=> other) == 0);
    return result;
}

dawn::Ref<dawn::Value> dawn::Value::operator!=( Value const& other ) const
{
    auto result = BoolValue::make();
    result->value = ((*this <=> other) != 0);
    return result;
}

dawn::Ref<dawn::Value> dawn::Value::operator<( Value const& other ) const
{
    auto result = BoolValue::make();
    result->value = ((*this <=> other) < 0);
    return result;
}

dawn::Ref<dawn::Value> dawn::Value::operator>( Value const& other ) const
{
    auto result = BoolValue::make();
    result->value = ((*this <=> other) > 0);
    return result;
}

dawn::Ref<dawn::Value> dawn::Value::operator<=( Value const& other ) const
{
    auto result = BoolValue::make();
    result->value = ((*this <=> other) <= 0);
    return result;
}

dawn::Ref<dawn::Value> dawn::Value::operator>=( Value const& other ) const
{
    auto result = BoolValue::make();
    result->value = ((*this <=> other) >= 0);
    return result;
}

dawn::Ref<dawn::Value> dawn::Value::operator!() const
{
    auto result = BoolValue::make();
    result->value = !to_bool();
    return result;
}

dawn::Ref<dawn::Value> dawn::Value::operator&&( Value const& other ) const
{
    auto result = BoolValue::make();
    result->value = to_bool() && other.to_bool();
    return result;
}

dawn::Ref<dawn::Value> dawn::Value::operator||( Value const& other ) const
{
    auto result = BoolValue::make();
    result->value = to_bool() || other.to_bool();
    return result;
}

dawn::Ref<dawn::Value> dawn::Value::operator>>( Value const& other ) const
{
    PANIC( "operator ~ for [", type(), "] not supported" );
}

dawn::Ref<dawn::Value> dawn::Value::operator~() const
{
    return IntValue{} >> *this;
}

dawn::Bool dawn::Value::to_bool() const
{
    PANIC( "bool conversion for [", type(), "] not supported" );
}

dawn::Int dawn::Value::to_int() const
{
    PANIC( "int conversion for [", type(), "] not supported" );
}

dawn::Float dawn::Value::to_float() const
{
    PANIC( "float conversion for [", type(), "] not supported" );
}

dawn::Char dawn::Value::to_char() const
{
    PANIC( "char conversion for [", type(), "] not supported" );
}

dawn::String dawn::Value::to_string() const
{
    PANIC( "string conversion for [", type(), "] not supported" );
}
