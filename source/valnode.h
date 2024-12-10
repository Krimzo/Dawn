#pragma once

#include "syntax.h"


namespace dawn
{
struct Node
{
    virtual ~Node() = default;
};

struct Value;

using RawValue = Ref<Value>;

struct Value
{
    virtual ~Value() = default;

    virtual StringRef const& type() const = 0;
    virtual RawValue clone() const = 0;

    virtual RawValue operator-() const;
    virtual RawValue operator+( Value const& other ) const;
    virtual RawValue operator-( Value const& other ) const;
    virtual RawValue operator*( Value const& other ) const;
    virtual RawValue operator/( Value const& other ) const;
    virtual RawValue operator^( Value const& other ) const;
    virtual RawValue operator%( Value const& other ) const;

    virtual Int operator<=>( Value const& other ) const;
    virtual RawValue operator==( Value const& other ) const final;
    virtual RawValue operator!=( Value const& other ) const final;
    virtual RawValue operator<( Value const& other ) const final;
    virtual RawValue operator>( Value const& other ) const final;
    virtual RawValue operator<=( Value const& other ) const final;
    virtual RawValue operator>=( Value const& other ) const final;

    virtual RawValue operator!() const final;
    virtual RawValue operator&&( Value const& other ) const final;
    virtual RawValue operator||( Value const& other ) const final;

    virtual RawValue operator>>( Value const& other ) const;
    virtual RawValue operator~() const final;

    virtual Bool to_bool() const;
    virtual Int to_int() const;
    virtual Float to_float() const;
    virtual Char to_char() const;
    virtual String to_string() const;
};

enum struct ValueKind
{
    LET = 0,
    VAR,
};

struct ValueBox
{
    ValueBox();
    ValueBox( ValueKind kind, RawValue const& value );

    RawValue const& value() const;
    void set_value( RawValue const& value );

private:
    ValueKind m_kind;
    Ref<RawValue> m_value_ref;

    void reapply_kind();
};
}
