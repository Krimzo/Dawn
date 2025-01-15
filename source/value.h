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
};

struct StructVal
{
    Struct* parent = nullptr;
    OMap<Int, Value> members;

    StructVal() = default;

    StructVal( StructVal const& other );
    StructVal& operator=( StructVal const& other );

    Value* get_member( Int id );
    Function* get_method( Int id, Bool unary );
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

enum struct ValueKind
{
    LET = 0,
    VAR,
};

struct Value
{
    Value() = default;
    explicit Value( Bool value, ValueKind kind = ValueKind::LET );
    explicit Value( Int value, ValueKind kind = ValueKind::LET );
    explicit Value( Float value, ValueKind kind = ValueKind::LET );
    explicit Value( Char value, ValueKind kind = ValueKind::LET );
    explicit Value( StringRef const& value, ValueKind kind = ValueKind::LET );
    explicit Value( Function const& value, ValueKind kind = ValueKind::LET );
    explicit Value( EnumVal const& value, ValueKind kind = ValueKind::LET );
    explicit Value( StructVal const& value, ValueKind kind = ValueKind::LET );
    explicit Value( ArrayVal const& value, ValueKind kind = ValueKind::LET );
    explicit Value( RangeVal const& value, ValueKind kind = ValueKind::LET );

    ValueKind kind() const;
    ValueType type() const;

    template<typename T>
    T& as() const
    {
        return m_regref.cast<T>().value();
    }

    void assign( Value const& other );

    Value clone() const;
    Value clone( ValueKind kind ) const;

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
    ValueKind m_kind = ValueKind::LET;
    ValueType m_type = ValueType::NOTHING;

    void reapply_kind();
};

std::ostream& operator<<( std::ostream& stream, ValueType type );
}
