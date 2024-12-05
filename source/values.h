#pragma once

#include "types.h"


namespace dawn
{
struct NothingValue : Value, Makeable<NothingValue>
{
    Ref<Value> operator+() const override;
    Ref<Value> operator-() const override;

    Ref<Value> operator+( Value const& other ) const override;
    Ref<Value> operator-( Value const& other ) const override;
    Ref<Value> operator*( Value const& other ) const override;
    Ref<Value> operator/( Value const& other ) const override;
    Ref<Value> operator^( Value const& other ) const override;
    Ref<Value> operator%( Value const& other ) const override;

    Bool to_bool() const override;
    Int to_int() const override;
    Float to_float() const override;
    Char to_char() const override;
    String to_string() const override;

    Ref<Value> clone() const override;
};

struct BoolValue : Value, Makeable<BoolValue>
{
    Bool value = {};

    Ref<Value> operator+() const override;
    Ref<Value> operator-() const override;

    Ref<Value> operator+( Value const& other ) const override;
    Ref<Value> operator-( Value const& other ) const override;
    Ref<Value> operator*( Value const& other ) const override;
    Ref<Value> operator/( Value const& other ) const override;
    Ref<Value> operator^( Value const& other ) const override;
    Ref<Value> operator%( Value const& other ) const override;

    Bool to_bool() const override;
    Int to_int() const override;
    Float to_float() const override;
    Char to_char() const override;
    String to_string() const override;

    Ref<Value> clone() const override;
};

struct IntValue : Value, Makeable<IntValue>
{
    Int value = {};

    Ref<Value> operator+() const override;
    Ref<Value> operator-() const override;

    Ref<Value> operator+( Value const& other ) const override;
    Ref<Value> operator-( Value const& other ) const override;
    Ref<Value> operator*( Value const& other ) const override;
    Ref<Value> operator/( Value const& other ) const override;
    Ref<Value> operator^( Value const& other ) const override;
    Ref<Value> operator%( Value const& other ) const override;

    Bool to_bool() const override;
    Int to_int() const override;
    Float to_float() const override;
    Char to_char() const override;
    String to_string() const override;

    Ref<Value> clone() const override;
};

struct FloatValue : Value, Makeable<FloatValue>
{
    Float value = {};

    Ref<Value> operator+() const override;
    Ref<Value> operator-() const override;

    Ref<Value> operator+( Value const& other ) const override;
    Ref<Value> operator-( Value const& other ) const override;
    Ref<Value> operator*( Value const& other ) const override;
    Ref<Value> operator/( Value const& other ) const override;
    Ref<Value> operator^( Value const& other ) const override;
    Ref<Value> operator%( Value const& other ) const override;

    Bool to_bool() const override;
    Int to_int() const override;
    Float to_float() const override;
    Char to_char() const override;
    String to_string() const override;

    Ref<Value> clone() const override;
};

struct CharValue : Value, Makeable<CharValue>
{
    Char value = {};

    Ref<Value> operator+() const override;
    Ref<Value> operator-() const override;

    Ref<Value> operator+( Value const& other ) const override;
    Ref<Value> operator-( Value const& other ) const override;
    Ref<Value> operator*( Value const& other ) const override;
    Ref<Value> operator/( Value const& other ) const override;
    Ref<Value> operator^( Value const& other ) const override;
    Ref<Value> operator%( Value const& other ) const override;

    Bool to_bool() const override;
    Int to_int() const override;
    Float to_float() const override;
    Char to_char() const override;
    String to_string() const override;

    Ref<Value> clone() const override;
};

struct StringValue : Value, Makeable<StringValue>
{
    String value = {};

    Ref<Value> operator+() const override;
    Ref<Value> operator-() const override;

    Ref<Value> operator+( Value const& other ) const override;
    Ref<Value> operator-( Value const& other ) const override;
    Ref<Value> operator*( Value const& other ) const override;
    Ref<Value> operator/( Value const& other ) const override;
    Ref<Value> operator^( Value const& other ) const override;
    Ref<Value> operator%( Value const& other ) const override;

    Bool to_bool() const override;
    Int to_int() const override;
    Float to_float() const override;
    Char to_char() const override;
    String to_string() const override;

    Ref<Value> clone() const override;
};

struct EnumValue : Value, Makeable<EnumValue>
{
    Ref<EnumType> parent;
    String key;

    Ref<Value> operator+() const override;
    Ref<Value> operator-() const override;

    Ref<Value> operator+( Value const& other ) const override;
    Ref<Value> operator-( Value const& other ) const override;
    Ref<Value> operator*( Value const& other ) const override;
    Ref<Value> operator/( Value const& other ) const override;
    Ref<Value> operator^( Value const& other ) const override;
    Ref<Value> operator%( Value const& other ) const override;

    Bool to_bool() const override;
    Int to_int() const override;
    Float to_float() const override;
    Char to_char() const override;
    String to_string() const override;

    Ref<Value> clone() const override;
};

struct StructValue : Value, Makeable<StructValue>
{
    Ref<StructType> parent;
    Map<String, Ref<Value>> members;

    Ref<Value> operator+() const override;
    Ref<Value> operator-() const override;

    Ref<Value> operator+( Value const& other ) const override;
    Ref<Value> operator-( Value const& other ) const override;
    Ref<Value> operator*( Value const& other ) const override;
    Ref<Value> operator/( Value const& other ) const override;
    Ref<Value> operator^( Value const& other ) const override;
    Ref<Value> operator%( Value const& other ) const override;

    Bool to_bool() const override;
    Int to_int() const override;
    Float to_float() const override;
    Char to_char() const override;
    String to_string() const override;

    Ref<Value> clone() const override;
};

struct ArrayValue : Value, Makeable<ArrayValue>
{
    Array<Ref<Value>> data = {};

    Ref<Value> operator+() const override;
    Ref<Value> operator-() const override;

    Ref<Value> operator+( Value const& other ) const override;
    Ref<Value> operator-( Value const& other ) const override;
    Ref<Value> operator*( Value const& other ) const override;
    Ref<Value> operator/( Value const& other ) const override;
    Ref<Value> operator^( Value const& other ) const override;
    Ref<Value> operator%( Value const& other ) const override;

    Bool to_bool() const override;
    Int to_int() const override;
    Float to_float() const override;
    Char to_char() const override;
    String to_string() const override;

    Ref<Value> clone() const override;
};
}
