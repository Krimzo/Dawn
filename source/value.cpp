#include "value.h"
#include "pool.h"
#include "engine.h"

// value.cpp use only
static const dawn::ID id_enum = dawn::kw_enum;
static const dawn::ID id_struct = dawn::kw_struct;

dawn::Bool dawn::FunctionValue::is_global() const
{
    return std::holds_alternative<AsGlobal>(data);
}

dawn::Bool dawn::FunctionValue::is_method() const
{
    return std::holds_alternative<AsMethod>(data);
}

dawn::Bool dawn::FunctionValue::is_lambda() const
{
    return std::holds_alternative<AsLambda>(data);
}

dawn::FunctionValue::AsGlobal& dawn::FunctionValue::as_global() const
{
    return const_cast<AsGlobal&>(std::get<AsGlobal>(data));
}

dawn::FunctionValue::AsMethod& dawn::FunctionValue::as_method() const
{
    return const_cast<AsMethod&>(std::get<AsMethod>(data));
}

dawn::FunctionValue::AsLambda& dawn::FunctionValue::as_lambda() const
{
    return const_cast<AsLambda&>(std::get<AsLambda>(data));
}

dawn::DFunction* dawn::FunctionValue::dfunction() const
{
    if (std::holds_alternative<AsGlobal>(data))
    {
        auto& func = std::get<AsGlobal>(data).func;
        if (std::holds_alternative<DFunction>(func))
            return const_cast<DFunction*>(&std::get<DFunction>(func));
    }
    else if (std::holds_alternative<AsMethod>(data))
    {
        auto& func = std::get<AsMethod>(data).func;
        if (std::holds_alternative<DFunction>(func))
            return const_cast<DFunction*>(&std::get<DFunction>(func));
    }
    else if (std::holds_alternative<AsLambda>(data))
    {
        auto& func = std::get<AsLambda>(data).func;
        if (std::holds_alternative<DFunction>(func))
            return const_cast<DFunction*>(&std::get<DFunction>(func));
    }
    return nullptr;
}

dawn::CFunction* dawn::FunctionValue::cfunction() const
{
    if (std::holds_alternative<AsGlobal>(data))
    {
        auto& func = std::get<AsGlobal>(data).func;
        if (std::holds_alternative<CFunction>(func))
            return const_cast<CFunction*>(&std::get<CFunction>(func));
    }
    else if (std::holds_alternative<AsMethod>(data))
    {
        auto& func = std::get<AsMethod>(data).func;
        if (std::holds_alternative<CFunction>(func))
            return const_cast<CFunction*>(&std::get<CFunction>(func));
    }
    else if (std::holds_alternative<AsLambda>(data))
    {
        auto& func = std::get<AsLambda>(data).func;
        if (std::holds_alternative<CFunction>(func))
            return const_cast<CFunction*>(&std::get<CFunction>(func));
    }
    return nullptr;
}

dawn::ArrayValue::ArrayValue(ArrayValue const& other)
{
    data.reserve(other.data.size());
    for (auto& value : other.data)
        data.push_back(value.clone());
}

dawn::ArrayValue& dawn::ArrayValue::operator=(ArrayValue const& other)
{
    if (this != &other)
    {
        data.clear();
        data.reserve(other.data.size());
        for (auto& value : other.data)
            data.push_back(value.clone());
    }
    return *this;
}

dawn::ArrayValue::ArrayValue(ArrayValue&& other) noexcept : data(std::move(other.data))
{
}

dawn::ArrayValue& dawn::ArrayValue::operator=(ArrayValue&& other) noexcept
{
    if (this != &other)
        data = std::move(other.data);
    return *this;
}

dawn::StructValue::StructValue(StructValue const& other) : parent_id(other.parent_id)
{
    fields.reserve(other.fields.size());
    for (auto& field : other.fields)
        fields.emplace_back(field.first, field.second.clone());
}

dawn::StructValue& dawn::StructValue::operator=(StructValue const& other)
{
    if (this != &other)
    {
        parent_id = other.parent_id;
        fields.clear();
        fields.reserve(other.fields.size());
        for (auto& field : other.fields)
            fields.emplace_back(field.first, field.second.clone());
    }
    return *this;
}

dawn::StructValue::StructValue(StructValue&& other) noexcept
    : parent_id(std::move(other.parent_id)), fields(std::move(other.fields))
{
}

dawn::StructValue& dawn::StructValue::operator=(StructValue&& other) noexcept
{
    if (this != &other)
    {
        parent_id = std::move(other.parent_id);
        fields = std::move(other.fields);
    }
    return *this;
}

dawn::Value* dawn::StructValue::get(ID id) const
{
    for (auto& field : fields)
        if (field.first == id)
            return const_cast<Value*>(&field.second);
    return nullptr;
}

dawn::Value::Value(Bool value, Location location) : m_regref(bool_pool().new_register().as<ValueInfo>())
{
    auto& storage = *m_regref.as<ValueStorage<Bool>>();
    storage.info.location = location;
    storage.info.type_id = id_bool;
    storage.info.is_const = true;
    storage.value = value;
}

dawn::Value::Value(Bool* value, Bool is_const, Location location) : m_regref(ptr_pool().new_register().as<ValueInfo>())
{
    auto& storage = *m_regref.as<ValueStorage<Ptr>>();
    storage.info.location = location;
    storage.info.type_id = id_bool;
    storage.info.is_const = is_const;
    storage.info.is_ptr = true;
    storage.value = value;
}

dawn::Value::Value(Int value, Location location) : m_regref(int_pool().new_register().as<ValueInfo>())
{
    auto& storage = *m_regref.as<ValueStorage<Int>>();
    storage.info.location = location;
    storage.info.type_id = id_int;
    storage.info.is_const = true;
    storage.value = value;
}

dawn::Value::Value(Int* value, Bool is_const, Location location) : m_regref(ptr_pool().new_register().as<ValueInfo>())
{
    auto& storage = *m_regref.as<ValueStorage<Ptr>>();
    storage.info.location = location;
    storage.info.type_id = id_int;
    storage.info.is_const = is_const;
    storage.info.is_ptr = true;
    storage.value = value;
}

dawn::Value::Value(Float value, Location location) : m_regref(float_pool().new_register().as<ValueInfo>())
{
    auto& storage = *m_regref.as<ValueStorage<Float>>();
    storage.info.location = location;
    storage.info.type_id = id_float;
    storage.info.is_const = true;
    storage.value = value;
}

dawn::Value::Value(Float* value, Bool is_const, Location location) : m_regref(ptr_pool().new_register().as<ValueInfo>())
{
    auto& storage = *m_regref.as<ValueStorage<Ptr>>();
    storage.info.location = location;
    storage.info.type_id = id_float;
    storage.info.is_const = is_const;
    storage.info.is_ptr = true;
    storage.value = value;
}

dawn::Value::Value(Char value, Location location) : m_regref(char_pool().new_register().as<ValueInfo>())
{
    auto& storage = *m_regref.as<ValueStorage<Char>>();
    storage.info.location = location;
    storage.info.type_id = id_char;
    storage.info.is_const = true;
    storage.value = value;
}

dawn::Value::Value(Char* value, Bool is_const, Location location) : m_regref(ptr_pool().new_register().as<ValueInfo>())
{
    auto& storage = *m_regref.as<ValueStorage<Ptr>>();
    storage.info.location = location;
    storage.info.type_id = id_char;
    storage.info.is_const = is_const;
    storage.info.is_ptr = true;
    storage.value = value;
}

dawn::Value::Value(String value, Location location) : m_regref(string_pool().new_register().as<ValueInfo>())
{
    auto& storage = *m_regref.as<ValueStorage<String>>();
    storage.info.location = location;
    storage.info.type_id = id_string;
    storage.info.is_const = true;
    storage.value = std::move(value);
}

dawn::Value::Value(String* value, Bool is_const, Location location)
    : m_regref(ptr_pool().new_register().as<ValueInfo>())
{
    auto& storage = *m_regref.as<ValueStorage<Ptr>>();
    storage.info.location = location;
    storage.info.type_id = id_string;
    storage.info.is_const = is_const;
    storage.info.is_ptr = true;
    storage.value = value;
}

dawn::Value::Value(RangeValue const& value, Location location) : m_regref(range_pool().new_register().as<ValueInfo>())
{
    auto& storage = *m_regref.as<ValueStorage<RangeValue>>();
    storage.info.location = location;
    storage.info.type_id = id_range;
    storage.info.is_const = true;
    storage.value = value;
}

dawn::Value::Value(RangeValue* value, Bool is_const, Location location)
    : m_regref(ptr_pool().new_register().as<ValueInfo>())
{
    auto& storage = *m_regref.as<ValueStorage<Ptr>>();
    storage.info.location = location;
    storage.info.type_id = id_range;
    storage.info.is_const = is_const;
    storage.info.is_ptr = true;
    storage.value = value;
}

dawn::Value::Value(FunctionValue const& value, Location location)
    : m_regref(function_pool().new_register().as<ValueInfo>())
{
    auto& storage = *m_regref.as<ValueStorage<FunctionValue>>();
    storage.info.location = location;
    storage.info.type_id = id_function;
    storage.info.is_const = true;
    storage.value = value;
}

dawn::Value::Value(FunctionValue* value, Bool is_const, Location location)
    : m_regref(ptr_pool().new_register().as<ValueInfo>())
{
    auto& storage = *m_regref.as<ValueStorage<Ptr>>();
    storage.info.location = location;
    storage.info.type_id = id_function;
    storage.info.is_const = is_const;
    storage.info.is_ptr = true;
    storage.value = value;
}

dawn::Value::Value(ArrayValue const& value, Location location) : m_regref(array_pool().new_register().as<ValueInfo>())
{
    auto& storage = *m_regref.as<ValueStorage<ArrayValue>>();
    storage.info.location = location;
    storage.info.type_id = id_array;
    storage.info.is_const = true;
    storage.value = value;
}

dawn::Value::Value(ArrayValue* value, Bool is_const, Location location)
    : m_regref(ptr_pool().new_register().as<ValueInfo>())
{
    auto& storage = *m_regref.as<ValueStorage<Ptr>>();
    storage.info.location = location;
    storage.info.type_id = id_array;
    storage.info.is_const = is_const;
    storage.info.is_ptr = true;
    storage.value = value;
}

dawn::Value::Value(EnumValue const& value, Location location) : m_regref(enum_pool().new_register().as<ValueInfo>())
{
    auto& storage = *m_regref.as<ValueStorage<EnumValue>>();
    storage.info.location = location;
    storage.info.type_id = id_enum;
    storage.info.is_const = true;
    storage.value = value;
}

dawn::Value::Value(EnumValue* value, Bool is_const, Location location)
    : m_regref(ptr_pool().new_register().as<ValueInfo>())
{
    auto& storage = *m_regref.as<ValueStorage<Ptr>>();
    storage.info.location = location;
    storage.info.type_id = id_enum;
    storage.info.is_const = is_const;
    storage.info.is_ptr = true;
    storage.value = value;
}

dawn::Value::Value(StructValue const& value, Location location) : m_regref(struct_pool().new_register().as<ValueInfo>())
{
    auto& storage = *m_regref.as<ValueStorage<StructValue>>();
    storage.info.location = location;
    storage.info.type_id = id_struct;
    storage.info.is_const = true;
    storage.value = value;
}

dawn::Value::Value(StructValue* value, Bool is_const, Location location)
    : m_regref(ptr_pool().new_register().as<ValueInfo>())
{
    auto& storage = *m_regref.as<ValueStorage<Ptr>>();
    storage.info.location = location;
    storage.info.type_id = id_struct;
    storage.info.is_const = is_const;
    storage.info.is_ptr = true;
    storage.value = value;
}

dawn::Bool& dawn::Value::as_bool() const
{
    const auto type = m_regref ? m_regref->type_id : id_void;
    if (type != id_void)
        ENGINE_PANIC(location(), "expected [", id_void, "] but got [", type, "]");
    return m_regref.as<ValueStorage<Bool>>()->get();
}

dawn::Int& dawn::Value::as_int() const
{
    const auto type = m_regref ? m_regref->type_id : id_void;
    if (type != id_int)
        ENGINE_PANIC(location(), "expected [", id_int, "] but got [", type, "]");
    return m_regref.as<ValueStorage<Int>>()->get();
}

dawn::Float& dawn::Value::as_float() const
{
    const auto type = m_regref ? m_regref->type_id : id_void;
    if (type != id_float)
        ENGINE_PANIC(location(), "expected [", id_float, "] but got [", type, "]");
    return m_regref.as<ValueStorage<Float>>()->get();
}

dawn::Char& dawn::Value::as_char() const
{
    const auto type = m_regref ? m_regref->type_id : id_void;
    if (type != id_char)
        ENGINE_PANIC(location(), "expected [", id_char, "] but got [", type, "]");
    return m_regref.as<ValueStorage<Char>>()->get();
}

dawn::String& dawn::Value::as_string() const
{
    const auto type = m_regref ? m_regref->type_id : id_void;
    if (type != id_string)
        ENGINE_PANIC(location(), "expected [", id_string, "] but got [", type, "]");
    return m_regref.as<ValueStorage<String>>()->get();
}

dawn::RangeValue& dawn::Value::as_range() const
{
    const auto type = m_regref ? m_regref->type_id : id_void;
    if (type != id_range)
        ENGINE_PANIC(location(), "expected [", id_range, "] but got [", type, "]");
    return m_regref.as<ValueStorage<RangeValue>>()->get();
}

dawn::FunctionValue& dawn::Value::as_function() const
{
    const auto type = m_regref ? m_regref->type_id : id_void;
    if (type != id_function)
        ENGINE_PANIC(location(), "expected [", id_function, "] but got [", type, "]");
    return m_regref.as<ValueStorage<FunctionValue>>()->get();
}

dawn::ArrayValue& dawn::Value::as_array() const
{
    const auto type = m_regref ? m_regref->type_id : id_void;
    if (type != id_array)
        ENGINE_PANIC(location(), "expected [", id_array, "] but got [", type, "]");
    return m_regref.as<ValueStorage<ArrayValue>>()->get();
}

dawn::EnumValue& dawn::Value::as_enum() const
{
    const auto type = m_regref ? m_regref->type_id : id_void;
    if (type != id_enum)
        ENGINE_PANIC(location(), "expected [", id_enum, "] but got [", type, "]");
    return m_regref.as<ValueStorage<EnumValue>>()->get();
}

dawn::StructValue& dawn::Value::as_struct() const
{
    const auto type = m_regref ? m_regref->type_id : id_void;
    if (type != id_struct)
        ENGINE_PANIC(location(), "expected [", id_struct, "] but got [", type, "]");
    return m_regref.as<ValueStorage<StructValue>>()->get();
}

dawn::Location dawn::Value::location() const
{
    return m_regref ? m_regref->location : Location{};
}

dawn::ID dawn::Value::type_id() const
{
    if (!m_regref)
        return id_void;
    if (m_regref->type_id == id_enum)
        return as_enum().parent_id;
    if (m_regref->type_id == id_struct)
        return as_struct().parent_id;
    return m_regref->type_id;
}

void dawn::Value::assign(Value const& other)
{
    if (is_const())
        ENGINE_PANIC(location(), "can not assign [", other.type_id(), "] to a const value");
    if (type_id() != other.type_id())
        ENGINE_PANIC(location(), "can not assign [", other.type_id(), "] to [", type_id(), "]");
    const ID type = m_regref ? m_regref->type_id : id_void;
    if (type == id_bool)
        as_bool() = other.as_bool();
    else if (type == id_int)
        as_int() = other.as_int();
    else if (type == id_float)
        as_float() = other.as_float();
    else if (type == id_char)
        as_char() = other.as_char();
    else if (type == id_string)
        as_string() = other.as_string();
    else if (type == id_range)
        as_range() = other.as_range();
    else if (type == id_function)
        as_function() = other.as_function();
    else if (type == id_array)
        as_array() = other.as_array();
    else if (type == id_enum)
        as_enum() = other.as_enum();
    else if (type == id_struct)
        as_struct() = other.as_struct();
    unlock_const();
}

dawn::Value dawn::Value::clone() const
{
    const ID type = m_regref ? m_regref->type_id : id_void;
    if (type == id_bool)
        return Value{as_bool(), location()};
    else if (type == id_int)
        return Value{as_int(), location()};
    else if (type == id_float)
        return Value{as_float(), location()};
    else if (type == id_char)
        return Value{as_char(), location()};
    else if (type == id_string)
        return Value{as_string(), location()};
    else if (type == id_range)
        return Value{as_range(), location()};
    else if (type == id_function)
        return Value{as_function(), location()};
    else if (type == id_array)
        return Value{as_array(), location()};
    else if (type == id_enum)
        return Value{as_enum(), location()};
    else if (type == id_struct)
        return Value{as_struct(), location()};
    else
        return Value{};
}

dawn::Bool dawn::Value::is_const() const
{
    return m_regref ? m_regref->is_const : true;
}

dawn::Value& dawn::Value::unlock_const()
{
    const ID type = m_regref ? m_regref->type_id : id_void;
    if (m_regref)
        m_regref->is_const = false;
    if (type == id_array)
        for (auto& entry : as_array().data)
            entry.unlock_const();
    else if (type == id_struct)
        for (auto& [_, field] : as_struct().fields)
            field.unlock_const();
    return *this;
}
