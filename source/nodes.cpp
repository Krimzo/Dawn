#include "nodes.h"


dawn::Ref<dawn::Node> dawn::make_value_literal( Ref<Value> const& value )
{
    auto result = std::make_shared<ValueNode>();
    result->value = value;
    return result;
}

dawn::Ref<dawn::Node> dawn::make_bool_literal( Bool value )
{
    auto result = std::make_shared<ValueNode>();
    result->value = make_bool_value( value );
    return result;
}

dawn::Ref<dawn::Node> dawn::make_int_literal( Int value )
{
    auto result = std::make_shared<ValueNode>();
    result->value = make_int_value( value );
    return result;
}

dawn::Ref<dawn::Node> dawn::make_float_literal( Float value )
{
    auto result = std::make_shared<ValueNode>();
    result->value = make_float_value( value );
    return result;
}

dawn::Ref<dawn::Node> dawn::make_char_literal( Char value )
{
    auto result = std::make_shared<ValueNode>();
    result->value = make_char_value( value );
    return result;
}

dawn::Ref<dawn::Node> dawn::make_string_literal( StringRef const& value )
{
    auto result = std::make_shared<ValueNode>();
    result->value = make_string_value( value );
    return result;
}

dawn::Ref<dawn::Value> dawn::make_bool_value( Bool value )
{
    auto result = BoolValue::make();
    result->value = value;
    return result;
}

dawn::Ref<dawn::Value> dawn::make_int_value( Int value )
{
    auto result = IntValue::make();
    result->value = value;
    return result;
}

dawn::Ref<dawn::Value> dawn::make_float_value( Float value )
{
    auto result = FloatValue::make();
    result->value = value;
    return result;
}

dawn::Ref<dawn::Value> dawn::make_char_value( Char value )
{
    auto result = CharValue::make();
    result->value = value;
    return result;
}

dawn::Ref<dawn::Value> dawn::make_string_value( StringRef const& value )
{
    auto result = StringValue::make();
    result->value = value;
    return result;
}
