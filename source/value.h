#pragma once

#include "err.h"
#include "type.h"
#include "salloc.h"

namespace dawn
{
struct RangeValue
{
    Int start = 0;
    Int end = 0;
    Bool inclusive = false;

    constexpr Bool empty() const
    {
        return inclusive ? false : (start == end);
    }
};

struct DFunction
{
    Vector<Arg> args;
    Scope body;
};

struct CFunction : Func<Value(Location, Engine&, Value const*, Int)>
{
    using Func<Value(Location, Engine&, Value const*, Int)>::function;
};

struct DGlobalFunc : DFunction
{
    ID id;
};

struct CGlobalFunc : CFunction
{
    ID id;
};

struct DMethodFunc : DFunction
{
    ID id;
    Holder<Value> self;
};

struct CMethodFunc : CFunction
{
    ID id;
    Holder<Value> self;
};

struct DLambdaFunc : DFunction
{
    RegisterRef<Frame> frame;
};

struct CLambdaFunc : CFunction
{
    RegisterRef<Frame> frame;
};

struct FunctionValue : Variant<DGlobalFunc, CGlobalFunc, DMethodFunc, CMethodFunc, DLambdaFunc, CLambdaFunc>
{
    Bool is_global() const;
    Bool is_method() const;
    Bool is_lambda() const;

    DFunction* dfunction() const;
    CFunction* cfunction() const;

    ID id() const;
    Value& self() const;
    RegisterRef<Frame>& frame() const;
};

struct ArrayValue
{
    Vector<Value> data;

    ArrayValue() = default;

    ArrayValue(ArrayValue const& other);
    ArrayValue& operator=(ArrayValue const& other);

    ArrayValue(ArrayValue&& other) noexcept;
    ArrayValue& operator=(ArrayValue&& other) noexcept;
};

struct EnumValue
{
    ID parent_id;
    ID key_id;
};

struct StructValue
{
    ID parent_id;
    Vector<Pair<ID, Value>> fields;

    StructValue() = default;

    StructValue(StructValue const& other);
    StructValue& operator=(StructValue const& other);

    StructValue(StructValue&& other) noexcept;
    StructValue& operator=(StructValue&& other) noexcept;

    Value* get(ID id) const;
};

struct alignas(8) ValueInfo
{
    Location location;
    ID type_id;
    Bool is_const = true;
    Bool is_ptr = false;
};

template <typename T> struct ValueStorage
{
    ValueInfo info{};
    T value{};

    constexpr T& get() const
    {
        static_assert(alignof(decltype(*this)) == alignof(ValueInfo), "Bad ValueStorage data alignment.");
        if (info.is_ptr)
            return *const_cast<T*>(static_cast<T const*>(reinterpret_cast<Ptr const&>(value)));
        else
            return const_cast<T&>(value);
    }
};

struct Value
{
    constexpr Value() = default;
    explicit Value(Bool value, Location location = {});
    explicit Value(Bool* value, Bool is_const, Location location = {});
    explicit Value(Int value, Location location = {});
    explicit Value(Int* value, Bool is_const, Location location = {});
    explicit Value(Float value, Location location = {});
    explicit Value(Float* value, Bool is_const, Location location = {});
    explicit Value(Char value, Location location = {});
    explicit Value(Char* value, Bool is_const, Location location = {});
    explicit Value(String value, Location location = {});
    explicit Value(String* value, Bool is_const, Location location = {});
    explicit Value(RangeValue const& value, Location location = {});
    explicit Value(RangeValue* value, Bool is_const, Location location = {});
    explicit Value(FunctionValue const& value, Location location = {});
    explicit Value(FunctionValue* value, Bool is_const, Location location = {});
    explicit Value(ArrayValue const& value, Location location = {});
    explicit Value(ArrayValue* value, Bool is_const, Location location = {});
    explicit Value(EnumValue const& value, Location location = {});
    explicit Value(EnumValue* value, Bool is_const, Location location = {});
    explicit Value(StructValue const& value, Location location = {});
    explicit Value(StructValue* value, Bool is_const, Location location = {});

    Bool& as_bool() const;
    Int& as_int() const;
    Float& as_float() const;
    Char& as_char() const;
    String& as_string() const;
    RangeValue& as_range() const;
    FunctionValue& as_function() const;
    ArrayValue& as_array() const;
    EnumValue& as_enum() const;
    StructValue& as_struct() const;

    Location location() const;
    ID type_id() const;

    void assign(Value const& other);
    Value clone() const;

    Bool is_const() const;
    Value& unlock_const();

  private:
    RegisterRef<ValueInfo> m_regref;
};
} // namespace dawn
