#include "nodes.h"


dawn::Ref<dawn::Node> dawn::make_nothing_node()
{
    return make_value_node( Value{} );
}

dawn::Ref<dawn::Node> dawn::make_bool_node( Bool value )
{
    return make_value_node( Value{ value } );
}

dawn::Ref<dawn::Node> dawn::make_int_node( Int value )
{
    return make_value_node( Value{ value } );
}

dawn::Ref<dawn::Node> dawn::make_float_node( Float value )
{
    return make_value_node( Value{ value } );
}

dawn::Ref<dawn::Node> dawn::make_char_node( Char value )
{
    return make_value_node( Value{ value } );
}

dawn::Ref<dawn::Node> dawn::make_string_node( StringRef const& value )
{
    return make_value_node( Value{ value } );
}

dawn::Ref<dawn::Node> dawn::make_value_node( Value const& value )
{
    auto result = std::make_shared<ValueNode>();
    result->value = value;
    return result;
}
