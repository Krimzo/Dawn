#pragma once

#include "types.h"


namespace dawn
{
struct NothingValue : Value
{
    StringRef const& type() const override;
    RawValue clone() const override;

    Bool to_bool() const override;
    Int to_int() const override;
    Float to_float() const override;
    Char to_char() const override;
    String to_string() const override;
};

struct BoolValue : Value
{
    Bool value = {};

    StringRef const& type() const override;
    RawValue clone() const override;

    Int operator<=>( Value const& other ) const override;

    Bool to_bool() const override;
    Int to_int() const override;
    Float to_float() const override;
    Char to_char() const override;
    String to_string() const override;
};

struct IntValue : Value
{
    Int value = {};

    StringRef const& type() const override;
    RawValue clone() const override;

    RawValue operator-() const override;
    RawValue operator+( Value const& other ) const override;
    RawValue operator-( Value const& other ) const override;
    RawValue operator*( Value const& other ) const override;
    RawValue operator/( Value const& other ) const override;
    RawValue operator^( Value const& other ) const override;
    RawValue operator%( Value const& other ) const override;

    Int operator<=>( Value const& other ) const override;
    RawValue operator>>( Value const& other ) const override;

    Bool to_bool() const override;
    Int to_int() const override;
    Float to_float() const override;
    Char to_char() const override;
    String to_string() const override;
};

struct FloatValue : Value
{
    Float value = {};

    StringRef const& type() const override;
    RawValue clone() const override;

    RawValue operator-() const override;
    RawValue operator+( Value const& other ) const override;
    RawValue operator-( Value const& other ) const override;
    RawValue operator*( Value const& other ) const override;
    RawValue operator/( Value const& other ) const override;
    RawValue operator^( Value const& other ) const override;
    RawValue operator%( Value const& other ) const override;

    Int operator<=>( Value const& other ) const override;

    Bool to_bool() const override;
    Int to_int() const override;
    Float to_float() const override;
    Char to_char() const override;
    String to_string() const override;
};

struct CharValue : Value
{
    Char value = {};

    StringRef const& type() const override;
    RawValue clone() const override;

    Int operator<=>( Value const& other ) const override;

    Bool to_bool() const override;
    Int to_int() const override;
    Float to_float() const override;
    Char to_char() const override;
    String to_string() const override;
};

struct StringValue : Value
{
    String value = {};

    StringRef const& type() const override;
    RawValue clone() const override;

    RawValue operator+( Value const& other ) const override;

    Int operator<=>( Value const& other ) const override;

    Bool to_bool() const override;
    Int to_int() const override;
    Float to_float() const override;
    Char to_char() const override;
    String to_string() const override;
};

struct EnumValue : Value
{
    Enum const* parent = nullptr;
    String key;

    StringRef const& type() const override;
    RawValue clone() const override;

    String to_string() const override;
};

struct StructValue : Value
{
    Struct const* parent = nullptr;
    Map<String, ValueBox> members;

    StringRef const& type() const override;
    RawValue clone() const override;

    String to_string() const override;
};

struct ArrayValue : Value
{
    Array<ValueBox> data = {};

    StringRef const& type() const override;
    RawValue clone() const override;

    RawValue operator+( Value const& other ) const override;

    Int operator<=>( Value const& other ) const override;

    String to_string() const override;
};

struct RangeValue : Value
{
    Int start_incl = 0;
    Int end_excl = 0;

    StringRef const& type() const override;
    RawValue clone() const override;

    String to_string() const override;
};

RawValue make_nothing_value();
RawValue make_bool_value( Bool value );
RawValue make_int_value( Int value );
RawValue make_float_value( Float value );
RawValue make_char_value( Char value );
RawValue make_string_value( StringRef const& value );
}
