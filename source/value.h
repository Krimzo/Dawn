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

struct CFunction : Func<Value( Location const&, Value*, Int )>
{
    using Func<Value( Location const&, Value*, Int )>::function;
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
    Int parent_id = 0;
    Int key_id = 0;
    Holder<Value> value;
};

struct StructValue
{
    Engine* engine;
    Int parent_id = 0;
    Map<Int, Value> members;

    inline StructValue( Engine* engine = nullptr )
        : engine( engine )
    {
    }

    StructValue( StructValue const& other );
    StructValue& operator=( StructValue const& other );

    StructValue( StructValue&& other ) noexcept;
    StructValue& operator=( StructValue&& other ) noexcept;

    Struct& parent() const;
    Value* get_member( Int id );
    FunctionValue* get_method( Int id, Bool has_no_args );
};

struct ArrayValue
{
    Vector<Value> data;

    ArrayValue() = default;

    ArrayValue( ArrayValue const& other );
    ArrayValue& operator=( ArrayValue const& other );

    ArrayValue( ArrayValue&& other ) noexcept;
    ArrayValue& operator=( ArrayValue&& other ) noexcept;
};

struct RangeValue
{
    Int start_incl = 0;
    Int end_excl = 0;

    constexpr Bool empty() const
    {
        return start_incl >= end_excl;
    }
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

    constexpr ValueType type() const { return m_type; }
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

    void assign( Location const& location, Value const& other );
    Value clone() const;

    Bool is_const() const;
    Value& unlock_const();

    Value un_plus( Location const& location ) const;
    Value un_minus( Location const& location ) const;
    Value op_add( Location const& location, Value const& other ) const;
    Value op_sub( Location const& location, Value const& other ) const;
    Value op_mul( Location const& location, Value const& other ) const;
    Value op_div( Location const& location, Value const& other ) const;
    Value op_pow( Location const& location, Value const& other ) const;
    Value op_mod( Location const& location, Value const& other ) const;

    Value op_cmpr( Location const& location, Value const& other ) const;
    Value op_eq( Location const& location, Value const& other ) const;
    Value op_neq( Location const& location, Value const& other ) const;
    Value op_less( Location const& location, Value const& other ) const;
    Value op_great( Location const& location, Value const& other ) const;
    Value op_lesseq( Location const& location, Value const& other ) const;
    Value op_greateq( Location const& location, Value const& other ) const;

    Value un_not( Location const& location ) const;
    Value op_and( Location const& location, Value const& other ) const;
    Value op_or( Location const& location, Value const& other ) const;

    Value op_range( Location const& location, Value const& other ) const;

    Bool to_bool( Location const& location ) const;
    Int to_int( Location const& location ) const;
    Float to_float( Location const& location ) const;
    Char to_char( Location const& location ) const;
    String to_string( Location const& location ) const;

private:
    struct Void {};

    RegisterRef<Void> m_regref;
    ValueType m_type = ValueType::NOTHING;
    Bool m_const = true;
};
}
