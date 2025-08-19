#pragma once

#include "err.h"
#include "type.h"
#include "salloc.h"


namespace dawn
{
struct DFunction
{
    Vector<Function::Arg> args;
    Scope body;
};

struct CFunction : Func<Value( Value*, Int )>
{
    using Func<Value( Value*, Int )>::function;
};

struct FunctionValue
{
    using Fn = Variant<DFunction, CFunction>;

    struct AsGlobal
    {
        Int id = 0;
        Fn func;
    };

    struct AsMethod
    {
        Int id = 0;
        Fn func;
        Holder<Value> self;
    };

    struct AsLambda
    {
        Fn func;
        RegisterRef<Frame> frame;
    };

    Variant<AsGlobal, AsMethod, AsLambda> data;

    Bool is_global() const;
    Bool is_method() const;
    Bool is_lambda() const;

    AsGlobal& as_global() const;
    AsMethod& as_method() const;
    AsLambda& as_lambda() const;

    DFunction* dfunction() const;
    CFunction* cfunction() const;
};

struct EnumValue
{
    Enum* parent = nullptr;
    Int key_id = 0;

    Value value( Engine& engine ) const;
};

struct StructValue
{
    Struct* parent = nullptr;
    Map<Int, Value> members;

    StructValue() = default;

    StructValue( StructValue const& other );
    StructValue& operator=( StructValue const& other );

    Value* get_member( Int id );
    FunctionValue* get_method( Int id, Bool has_no_args );
};

struct ArrayValue
{
    Vector<Value> data;

    ArrayValue() = default;

    ArrayValue( ArrayValue const& other );
    ArrayValue& operator=( ArrayValue const& other );
};

struct RangeValue
{
    Int start_incl = 0;
    Int end_excl = 0;
};

struct Value
{
    Value() = default;
    explicit Value( Bool value );
    explicit Value( Int value );
    explicit Value( Float value );
    explicit Value( Char value );
    explicit Value( StringRef const& value );
    explicit Value( FunctionValue const& value );
    explicit Value( EnumValue const& value );
    explicit Value( StructValue const& value );
    explicit Value( ArrayValue const& value );
    explicit Value( RangeValue const& value );

    inline ValueType type() const { return m_type; }
    inline Bool& as_bool() const { return m_regref.cast<Bool>().value(); }
    inline Int& as_int() const { return m_regref.cast<Int>().value(); }
    inline Float& as_float() const { return m_regref.cast<Float>().value(); }
    inline Char& as_char() const { return m_regref.cast<Char>().value(); }
    inline String& as_string() const { return m_regref.cast<String>().value(); }
    inline FunctionValue& as_function() const { return m_regref.cast<FunctionValue>().value(); }
    inline EnumValue& as_enum() const { return m_regref.cast<EnumValue>().value(); }
    inline StructValue& as_struct() const { return m_regref.cast<StructValue>().value(); }
    inline ArrayValue& as_array() const { return m_regref.cast<ArrayValue>().value(); }
    inline RangeValue& as_range() const { return m_regref.cast<RangeValue>().value(); }

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
}
