#pragma once

#include "err.h"
#include "types.h"
#include "memory.h"


namespace dawn
{
struct EnumVal
{
    Enum const* parent = nullptr;
    String key;
};

struct StructVal
{
    Struct const* parent = nullptr;
    Map<String, ValueRef> members;

    StructVal() = default;

    StructVal( StructVal const& other );
    StructVal& operator=( StructVal const& other );
};

struct ArrayVal
{
    Array<ValueRef> data;

    ArrayVal() = default;

    ArrayVal( ArrayVal const& other );
    ArrayVal& operator=( ArrayVal const& other );
};

struct RangeVal
{
    Int start_incl = 0;
    Int end_excl = 0;
};

enum struct ValueType
{
    NOTHING,
    BOOL,
    INT,
    FLOAT,
    CHAR,
    STRING,
    ENUM,
    STRUCT,
    ARRAY,
    RANGE,
};

struct Value
{
    Value();
    Value( Bool value );
    Value( Int value );
    Value( Float value );
    Value( Char value );
    Value( StringRef const& value );
    Value( EnumVal const& value );
    Value( StructVal const& value );
    Value( ArrayVal const& value );
    Value( RangeVal const& value );

    constexpr ValueType type() const
    {
        return static_cast<ValueType>(m_value.index());
    }

    template<typename T>
    constexpr T& as()
    {
        return std::get<T>( m_value );
    }

    template<typename T>
    constexpr T const& as() const
    {
        return std::get<T>( m_value );
    }

    Value operator+() const;
    Value operator-() const;
    Value operator+( Value const& other ) const;
    Value operator-( Value const& other ) const;
    Value operator*( Value const& other ) const;
    Value operator/( Value const& other ) const;
    Value operator^( Value const& other ) const;
    Value operator%( Value const& other ) const;

    Value operator==( Value const& other ) const;
    Value operator!=( Value const& other ) const;
    Value operator<( Value const& other ) const;
    Value operator>( Value const& other ) const;
    Value operator<=( Value const& other ) const;
    Value operator>=( Value const& other ) const;

    Value operator!() const;
    Value operator&&( Value const& other ) const;
    Value operator||( Value const& other ) const;

    Value operator~() const;
    Value operator>>( Value const& other ) const;

    Bool to_bool() const;
    Int to_int() const;
    Float to_float() const;
    Char to_char() const;
    String to_string() const;

private:
    Variant<
        nullptr_t,
        Bool,
        Int,
        Float,
        Char,
        String,
        EnumVal,
        StructVal,
        ArrayVal,
        RangeVal
    > m_value;
};

enum struct ValueKind
{
    LET = 0,
    VAR,
};

struct ValueRef
{
    ValueRef();
    ValueRef( Value const& value, ValueKind kind = ValueKind::LET );

    ValueKind kind() const;

    Value const& value() const;
    void set_value( Value const& value );

private:
    RegisterRef<Value> m_register;
    ValueKind m_kind;

    void reapply_kind();
};

std::wostream& operator<<( std::wostream& stream, ValueType type );
}
