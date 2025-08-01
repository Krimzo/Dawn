#pragma once

#include "err.h"
#include "type.h"
#include "salloc.h"


namespace dawn
{
struct Engine;

struct EnumVal
{
    Enum* parent = nullptr;
    Int key_id = 0;

    Value value( Engine& engine ) const;
};

struct StructVal
{
    Struct* parent = nullptr;
    OMap<Int, Value> members;

    StructVal() = default;

    StructVal( StructVal const& other );
    StructVal& operator=( StructVal const& other );

    Value* get_member( Int id );
    Function* get_method( Int id, Bool has_no_args );
};

struct ArrayVal
{
    Vector<Value> data;

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
    NOTHING = 0,
    BOOL,
    INT,
    FLOAT,
    CHAR,
    STRING,
    FUNCTION,
    ENUM,
    STRUCT,
    ARRAY,
    RANGE,
    _COUNT,
};

struct Value
{
    Value() = default;
    explicit Value( Bool value );
    explicit Value( Int value );
    explicit Value( Float value );
    explicit Value( Char value );
    explicit Value( StringRef const& value );
    explicit Value( Function const& value );
    explicit Value( EnumVal const& value );
    explicit Value( StructVal const& value );
    explicit Value( ArrayVal const& value );
    explicit Value( RangeVal const& value );

    inline ValueType type() const { return m_type; }
    inline Bool& as_bool() const { return m_regref.cast<Bool>().value(); }
    inline Int& as_int() const { return m_regref.cast<Int>().value(); }
    inline Float& as_float() const { return m_regref.cast<Float>().value(); }
    inline Char& as_char() const { return m_regref.cast<Char>().value(); }
    inline String& as_string() const { return m_regref.cast<String>().value(); }
    inline Function& as_function() const { return m_regref.cast<Function>().value(); }
    inline EnumVal& as_enum() const { return m_regref.cast<EnumVal>().value(); }
    inline StructVal& as_struct() const { return m_regref.cast<StructVal>().value(); }
    inline ArrayVal& as_array() const { return m_regref.cast<ArrayVal>().value(); }
    inline RangeVal& as_range() const { return m_regref.cast<RangeVal>().value(); }

    void assign( Value const& other );
    Value clone() const;

    Bool is_const() const;
    Value& unlock_const();

    Value un_plus( Engine& engine ) const;
    Value un_minus( Engine& engine ) const;
    Value op_add( Engine& engine, Value const& other ) const;
    Value op_sub( Engine& engine, Value const& other ) const;
    Value op_mul( Engine& engine, Value const& other ) const;
    Value op_div( Engine& engine, Value const& other ) const;
    Value op_pow( Engine& engine, Value const& other ) const;
    Value op_mod( Engine& engine, Value const& other ) const;

    Value op_cmpr( Engine& engine, Value const& other ) const;
    Value op_eq( Engine& engine, Value const& other ) const;
    Value op_neq( Engine& engine, Value const& other ) const;
    Value op_less( Engine& engine, Value const& other ) const;
    Value op_great( Engine& engine, Value const& other ) const;
    Value op_lesseq( Engine& engine, Value const& other ) const;
    Value op_greateq( Engine& engine, Value const& other ) const;

    Value un_not( Engine& engine ) const;
    Value op_and( Engine& engine, Value const& other ) const;
    Value op_or( Engine& engine, Value const& other ) const;

    Value op_range( Engine& engine, Value const& other ) const;

    Bool to_bool( Engine& engine ) const;
    Int to_int( Engine& engine ) const;
    Float to_float( Engine& engine ) const;
    Char to_char( Engine& engine ) const;
    String to_string( Engine& engine ) const;

private:
    struct Void {};

    RegisterRef<Void> m_regref;
    ValueType m_type = ValueType::NOTHING;
    Bool m_const = true;
};

std::ostream& operator<<( std::ostream& stream, ValueType type );
}
