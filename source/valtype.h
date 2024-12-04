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

    virtual Ref<Value> operator+() const = 0;
    virtual Ref<Value> operator-() const = 0;

    virtual Ref<Value> operator+( Value const& other ) const = 0;
    virtual Ref<Value> operator-( Value const& other ) const = 0;
    virtual Ref<Value> operator*( Value const& other ) const = 0;
    virtual Ref<Value> operator/( Value const& other ) const = 0;
    virtual Ref<Value> operator^( Value const& other ) const = 0;
    virtual Ref<Value> operator%( Value const& other ) const = 0;

    virtual Bool to_bool() const = 0;
    virtual Int to_int() const = 0;
    virtual Float to_float() const = 0;
    virtual Char to_char() const = 0;
    virtual String to_string() const = 0;

    virtual Ref<Value> clone() const = 0;
};

struct Node
{
    virtual ~Node() = default;
};
}
