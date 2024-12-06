#pragma once

#include "types.h"


namespace dawn
{
struct NothingValue : Value, Makeable<NothingValue>
{
    StringRef const& type() const override;
    Ref<Value> clone() const override;

    Bool to_bool() const override;
    Int to_int() const override;
    Float to_float() const override;
    Char to_char() const override;
    String to_string() const override;
};

struct BoolValue : Value, Makeable<BoolValue>
{
    Bool value = {};

    StringRef const& type() const override;
    Ref<Value> clone() const override;

    Int operator<=>( Value const& other ) const override;

    Bool to_bool() const override;
    Int to_int() const override;
    Float to_float() const override;
    Char to_char() const override;
    String to_string() const override;
};

struct IntValue : Value, Makeable<IntValue>
{
    Int value = {};

    StringRef const& type() const override;
    Ref<Value> clone() const override;

    Ref<Value> operator-() const override;
    Ref<Value> operator+( Value const& other ) const override;
    Ref<Value> operator-( Value const& other ) const override;
    Ref<Value> operator*( Value const& other ) const override;
    Ref<Value> operator/( Value const& other ) const override;
    Ref<Value> operator^( Value const& other ) const override;
    Ref<Value> operator%( Value const& other ) const override;

    Int operator<=>( Value const& other ) const override;
    Ref<Value> operator>>( Value const& other ) const override;

    Bool to_bool() const override;
    Int to_int() const override;
    Float to_float() const override;
    Char to_char() const override;
    String to_string() const override;
};

struct FloatValue : Value, Makeable<FloatValue>
{
    Float value = {};

    StringRef const& type() const override;
    Ref<Value> clone() const override;

    Ref<Value> operator-() const override;
    Ref<Value> operator+( Value const& other ) const override;
    Ref<Value> operator-( Value const& other ) const override;
    Ref<Value> operator*( Value const& other ) const override;
    Ref<Value> operator/( Value const& other ) const override;
    Ref<Value> operator^( Value const& other ) const override;
    Ref<Value> operator%( Value const& other ) const override;

    Int operator<=>( Value const& other ) const override;

    Bool to_bool() const override;
    Int to_int() const override;
    Float to_float() const override;
    Char to_char() const override;
    String to_string() const override;
};

struct CharValue : Value, Makeable<CharValue>
{
    Char value = {};

    StringRef const& type() const override;
    Ref<Value> clone() const override;

    Int operator<=>( Value const& other ) const override;

    Bool to_bool() const override;
    Int to_int() const override;
    Float to_float() const override;
    Char to_char() const override;
    String to_string() const override;
};

struct StringValue : Value, Makeable<StringValue>
{
    String value = {};

    StringRef const& type() const override;
    Ref<Value> clone() const override;

    Ref<Value> operator+( Value const& other ) const override;

    Int operator<=>( Value const& other ) const override;

    Bool to_bool() const override;
    Int to_int() const override;
    Float to_float() const override;
    Char to_char() const override;
    String to_string() const override;
};

struct EnumValue : Value, Makeable<EnumValue>
{
    EnumType const* parent = nullptr;
    String key;

    StringRef const& type() const override;
    Ref<Value> clone() const override;

    String to_string() const override;
};

struct StructValue : Value, Makeable<StructValue>
{
    StructType const* parent = nullptr;
    Map<String, Ref<Value>> members;

    StringRef const& type() const override;
    Ref<Value> clone() const override;

    String to_string() const override;
};

struct ArrayValue : Value, Makeable<ArrayValue>
{
    Array<Ref<Value>> data = {};

    StringRef const& type() const override;
    Ref<Value> clone() const override;

    Ref<Value> operator+( Value const& other ) const override;

    Int operator<=>( Value const& other ) const override;

    String to_string() const override;
};

struct RangeValue : Value, Makeable<RangeValue>
{
    Int start_incl = 0;
    Int end_excl = 0;

    StringRef const& type() const override;
    Ref<Value> clone() const override;

    String to_string() const override;
};
}
