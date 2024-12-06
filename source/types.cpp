#include "types.h"
#include "values.h"


dawn::Ref<dawn::Value> dawn::BoolType::construct() const
{
    return BoolValue::make();
}

dawn::Ref<dawn::Value> dawn::IntType::construct() const
{
    return IntValue::make();
}

dawn::Ref<dawn::Value> dawn::FloatType::construct() const
{
    return FloatValue::make();
}

dawn::Ref<dawn::Value> dawn::CharType::construct() const
{
    return CharValue::make();
}

dawn::Ref<dawn::Value> dawn::StringType::construct() const
{
    return StringValue::make();
}

dawn::Ref<dawn::Value> dawn::EnumType::construct() const
{
    auto result = EnumValue::make();
    result->parent = this;
    result->key = keys.front().first;
    return result;
}

dawn::Ref<dawn::Value> dawn::LayerType::construct() const
{
    PANIC( "layers can't be constructed" );
}

dawn::Ref<dawn::Value> dawn::StructType::construct() const
{
    auto result = StructValue::make();
    result->parent = this;
    return result;
}

dawn::Ref<dawn::Value> dawn::ArrayType::construct() const
{
    return ArrayValue::make();
}

dawn::Ref<dawn::Value> dawn::RangeType::construct() const
{
    return RangeValue::make();
}

dawn::Ref<dawn::Value> dawn::RefType::construct() const
{
    PANIC( "references can't be constructed" );
}
