#pragma once

#include "err.h"
#include "type.h"
#include "salloc.h"


namespace dawn
{
struct Nothing
{
    constexpr Nothing() = default;
};

struct DFunction
{
    Vector<Function::Arg> args;
    Scope body;
};

struct CFunction : Func<Value( Location const&, Engine&, Value*, Int )>
{
    using Func<Value( Location const&, Engine&, Value*, Int )>::function;
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
    Int parent_id = 0;
    Map<Int, Value> fields;
    Map<Int, Value> methods;

    StructValue() = default;

    StructValue( StructValue const& other );
    StructValue& operator=( StructValue const& other );

    StructValue( StructValue&& other ) noexcept;
    StructValue& operator=( StructValue&& other ) noexcept;

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
    constexpr Value( Nothing value = {} ) {}
    explicit Value( Bool value, Location const& location );
    explicit Value( Int value, Location const& location );
    explicit Value( Float value, Location const& location );
    explicit Value( Char value, Location const& location );
    explicit Value( StringRef const& value, Location const& location );
    explicit Value( FunctionValue const& value, Location const& location );
    explicit Value( EnumValue const& value, Location const& location );
    explicit Value( StructValue const& value, Location const& location );
    explicit Value( ArrayValue const& value, Location const& location );
    explicit Value( RangeValue const& value, Location const& location );

    Nothing as_nothing() const;
    Bool& as_bool() const;
    Int& as_int() const;
    Float& as_float() const;
    Char& as_char() const;
    String& as_string() const;
    FunctionValue& as_function() const;
    EnumValue& as_enum() const;
    StructValue& as_struct() const;
    ArrayValue& as_array() const;
    RangeValue& as_range() const;

    constexpr Location const& location() const { return m_location; }
    constexpr ValueType type() const { return m_type; }
    Int type_id() const;

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

    Value un_not() const;
    Value op_and( Value const& other ) const;
    Value op_or( Value const& other ) const;

    Value op_range( Engine& engine, Value const& other ) const;

    Nothing to_nothing( Engine& engine ) const;
    Bool to_bool( Engine& engine ) const;
    Int to_int( Engine& engine ) const;
    Float to_float( Engine& engine ) const;
    Char to_char( Engine& engine ) const;
    String to_string( Engine& engine ) const;
    FunctionValue to_function( Engine& engine ) const;
    ArrayValue to_array( Engine& engine ) const;
    RangeValue to_range( Engine& engine ) const;

private:
    struct Void {};

    RegisterRef<Void> m_regref;
    Location m_location;
    ValueType m_type = ValueType::NOTHING;
    Bool m_const = true;
};
}
