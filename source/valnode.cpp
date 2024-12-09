#include "valnode.h"
#include "values.h"


dawn::RawValue dawn::Value::operator-() const
{
    PANIC( "unary - for [", type(), "] not supported" );
}

dawn::RawValue dawn::Value::operator+( Value const& other ) const
{
    PANIC( "[", type(), "] + [", other.type(), "] not supported" );
}

dawn::RawValue dawn::Value::operator-( Value const& other ) const
{
    PANIC( "[", type(), "] - [", other.type(), "] not supported" );
}

dawn::RawValue dawn::Value::operator*( Value const& other ) const
{
    PANIC( "[", type(), "] * [", other.type(), "] not supported" );
}

dawn::RawValue dawn::Value::operator/( Value const& other ) const
{
    PANIC( "[", type(), "] / [", other.type(), "] not supported" );
}

dawn::RawValue dawn::Value::operator^( Value const& other ) const
{
    PANIC( "[", type(), "] ^ [", other.type(), "] not supported" );
}

dawn::RawValue dawn::Value::operator%( Value const& other ) const
{
    PANIC( "[", type(), "] % [", other.type(), "] not supported" );
}

dawn::Int dawn::Value::operator<=>( Value const& other ) const
{
    PANIC( "[", type(), "] <=> [", other.type(), "] not supported" );
}

dawn::RawValue dawn::Value::operator==( Value const& other ) const
{
    return make_bool_value( (*this <=> other) == 0 );
}

dawn::RawValue dawn::Value::operator!=( Value const& other ) const
{
    return make_bool_value( (*this <=> other) != 0 );
}

dawn::RawValue dawn::Value::operator<( Value const& other ) const
{
    return make_bool_value( (*this <=> other) < 0 );
}

dawn::RawValue dawn::Value::operator>( Value const& other ) const
{
    return make_bool_value( (*this <=> other) > 0 );
}

dawn::RawValue dawn::Value::operator<=( Value const& other ) const
{
    return make_bool_value( (*this <=> other) <= 0 );
}

dawn::RawValue dawn::Value::operator>=( Value const& other ) const
{
    return make_bool_value( (*this <=> other) >= 0 );
}

dawn::RawValue dawn::Value::operator!() const
{
    return make_bool_value( !to_bool() );
}

dawn::RawValue dawn::Value::operator&&( Value const& other ) const
{
    return make_bool_value( to_bool() && other.to_bool() );
}

dawn::RawValue dawn::Value::operator||( Value const& other ) const
{
    return make_bool_value( to_bool() || other.to_bool() );
}

dawn::RawValue dawn::Value::operator>>( Value const& other ) const
{
    PANIC( "operator ~ for [", type(), "] not supported" );
}

dawn::RawValue dawn::Value::operator~() const
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

dawn::ValueBox::ValueBox()
    : ValueBox( Type::VAR )
{}

dawn::ValueBox::ValueBox( Type type )
    : m_type( type )
{
    m_value_ref = std::make_shared<RawValue>();
    set_value( make_nothing_value() );
}

dawn::ValueBox::ValueBox( Type type, Ref<RawValue> const& value_ref )
    : m_type( type ), m_value_ref( value_ref )
{}

dawn::RawValue const& dawn::ValueBox::get_value() const
{
    return *m_value_ref;
}

void dawn::ValueBox::set_value( RawValue const& value )
{
    if ( !value )
        PANIC( "Cannot set null value" );

    if ( m_type == Type::LET )
        PANIC( "Cannot set value of a let variable" );

    (*m_value_ref) = value;
}
