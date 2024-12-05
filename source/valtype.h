#pragma once

#include "syntax.h"


namespace dawn
{
struct Type
{
    String name;

    virtual ~Type() = default;
};

struct Value
{
    virtual ~Value() = default;

    virtual StringRef const& type() const = 0;
    virtual Ref<Value> clone() const = 0;

    virtual Ref<Value> operator-() const;
    virtual Ref<Value> operator+( Value const& other ) const;
    virtual Ref<Value> operator-( Value const& other ) const;
    virtual Ref<Value> operator*( Value const& other ) const;
    virtual Ref<Value> operator/( Value const& other ) const;
    virtual Ref<Value> operator^( Value const& other ) const;
    virtual Ref<Value> operator%( Value const& other ) const;

    virtual Int operator<=>( Value const& other ) const;
    virtual Ref<Value> operator==( Value const& other ) const final;
    virtual Ref<Value> operator!=( Value const& other ) const final;
    virtual Ref<Value> operator<( Value const& other ) const final;
    virtual Ref<Value> operator>( Value const& other ) const final;
    virtual Ref<Value> operator<=( Value const& other ) const final;
    virtual Ref<Value> operator>=( Value const& other ) const final;

    virtual Ref<Value> operator!() const final;
    virtual Ref<Value> operator&&( Value const& other ) const final;
    virtual Ref<Value> operator||( Value const& other ) const final;

    virtual Ref<Value> operator>>( Value const& other ) const;
    virtual Ref<Value> operator~() const final;

    virtual Bool to_bool() const;
    virtual Int to_int() const;
    virtual Float to_float() const;
    virtual Char to_char() const;
    virtual String to_string() const;
};

struct Node
{
    virtual ~Node() = default;
};
}
