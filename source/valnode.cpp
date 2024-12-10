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
    : ValueBox( ValueKind::LET, make_nothing_value() )
{}

dawn::ValueBox::ValueBox( ValueKind kind, RawValue const& value )
{
    m_value_ref = std::make_shared<RawValue>( value );
    m_kind = ValueKind::VAR;
    set_value( value );
    m_kind = kind;
    reapply_kind();
}

dawn::RawValue const& dawn::ValueBox::value() const
{
    return *m_value_ref;
}

void dawn::ValueBox::set_value( RawValue const& value )
{
    if ( !value )
        PANIC( "Cannot set null value" );

    if ( m_kind == ValueKind::LET )
        PANIC( "Cannot set value of a let variable" );

    (*m_value_ref) = value->clone();
}

void dawn::ValueBox::reapply_kind()
{
    if ( auto array_value = dynamic_cast<ArrayValue*>(m_value_ref->get()) )
    {
        for ( auto& value : array_value->data )
        {
            value.m_kind = this->m_kind;
            value.reapply_kind();
        }
    }
    else if ( auto struct_value = dynamic_cast<StructValue*>(m_value_ref->get()) )
    {
        for ( auto& [_, member] : struct_value->members )
        {
            member.m_kind = this->m_kind;
            member.reapply_kind();
        }
    }
}
