#pragma once

#include "err.h"
#include "types.h"
#include "memory.h"
#include "storage.h"


namespace dawn
{
struct EnumVal
{
    Enum* parent = nullptr;
    ID key;
};

struct StructVal
{
    Struct* parent = nullptr;
    Map<Int, ValueRef> members;

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

consteval size_t max_val_size()
{
    return std::max( {
        sizeof( Bool ),
        sizeof( Int ),
        sizeof( Float ),
        sizeof( Char ),
        sizeof( String ),
        sizeof( EnumVal ),
        sizeof( StructVal ),
        sizeof( ArrayVal ),
        sizeof( RangeVal ) } );
}

consteval size_t max_val_align()
{
    return std::max( {
        alignof(Bool),
        alignof(Int),
        alignof(Float),
        alignof(Char),
        alignof(String),
        alignof(EnumVal),
        alignof(StructVal),
        alignof(ArrayVal),
        alignof(RangeVal) } );
}

enum struct ValueType
{
    NOTHING = 0,
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

struct ValueHandler
{
    template<typename T>
    static consteval ValueType type()
    {
        if constexpr ( std::is_same_v<T, Bool> )
            return ValueType::BOOL;

        else if constexpr ( std::is_same_v<T, Int> )
            return ValueType::INT;

        else if constexpr ( std::is_same_v<T, Float> )
            return ValueType::FLOAT;

        else if constexpr ( std::is_same_v<T, Char> )
            return ValueType::CHAR;

        else if constexpr ( std::is_same_v<T, String> )
            return ValueType::STRING;

        else if constexpr ( std::is_same_v<T, EnumVal> )
            return ValueType::ENUM;

        else if constexpr ( std::is_same_v<T, StructVal> )
            return ValueType::STRUCT;

        else if constexpr ( std::is_same_v<T, ArrayVal> )
            return ValueType::ARRAY;

        else if constexpr ( std::is_same_v<T, RangeVal> )
            return ValueType::RANGE;

        else
            static_assert(false, "Invalid value type");
    }

    static constexpr void copy( ValueType type, void* to, void const* from )
    {
        switch ( type )
        {
        case ValueType::BOOL:
            new (to) Bool( *static_cast<Bool const*>(from) );
            break;

        case ValueType::INT:
            new (to) Int( *static_cast<Int const*>(from) );
            break;

        case ValueType::FLOAT:
            new (to) Float( *static_cast<Float const*>(from) );
            break;

        case ValueType::CHAR:
            new (to) Char( *static_cast<Char const*>(from) );
            break;

        case ValueType::STRING:
            new (to) String( *static_cast<String const*>(from) );
            break;

        case ValueType::ENUM:
            new (to) EnumVal( *static_cast<EnumVal const*>(from) );
            break;

        case ValueType::STRUCT:
            new (to) StructVal( *static_cast<StructVal const*>(from) );
            break;

        case ValueType::ARRAY:
            new (to) ArrayVal( *static_cast<ArrayVal const*>(from) );
            break;

        case ValueType::RANGE:
            new (to) RangeVal( *static_cast<RangeVal const*>(from) );
            break;
        }
    }

    static constexpr void destruct( ValueType type, void* ptr )
    {
        switch ( type )
        {
        case ValueType::BOOL:
            static_cast<Bool*>(ptr)->~Bool();
            break;

        case ValueType::INT:
            static_cast<Int*>(ptr)->~Int();
            break;

        case ValueType::FLOAT:
            static_cast<Float*>(ptr)->~Float();
            break;

        case ValueType::CHAR:
            static_cast<Char*>(ptr)->~Char();
            break;

        case ValueType::STRING:
            static_cast<String*>(ptr)->~String();
            break;

        case ValueType::ENUM:
            static_cast<EnumVal*>(ptr)->~EnumVal();
            break;

        case ValueType::STRUCT:
            static_cast<StructVal*>(ptr)->~StructVal();
            break;

        case ValueType::ARRAY:
            static_cast<ArrayVal*>(ptr)->~ArrayVal();
            break;

        case ValueType::RANGE:
            static_cast<RangeVal*>(ptr)->~RangeVal();
            break;
        }
    }
};

struct Value : private Storage<max_val_size(), max_val_align(), ValueType, ValueHandler>
{
    Value() = default;
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
        return Storage::type();
    }

    template<typename T>
    constexpr T const& as() const
    {
        return Storage::as<T>();
    }

    template<typename T>
    constexpr T& as()
    {
        return Storage::as<T>();
    }

    template<typename T, typename... Args>
    constexpr T& store( Args const&... args )
    {
        return Storage::emplace<T>( args... );
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
};

enum struct ValueKind
{
    LET = 0,
    VAR,
};

struct ValueRef
{
    ValueRef() = default;
    ValueRef( Value const& value, ValueKind kind = ValueKind::LET );

    ValueKind kind() const;

    Value const& value() const;
    void set_value( Value const& value );

private:
    RegisterRef<Value> m_regref;
    ValueKind m_kind = ValueKind::LET;

    void reapply_kind();
};

std::ostream& operator<<( std::ostream& stream, ValueType type );
}
