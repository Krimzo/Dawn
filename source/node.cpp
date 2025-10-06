#include "node.h"


dawn::Node dawn::make_nothing_node()
{
    return make_value_node( Value{} );
}

dawn::Node dawn::make_bool_node( Location const& location, Bool value )
{
    return make_value_node( Value{ value, location } );
}

dawn::Node dawn::make_int_node( Location const& location, Int value )
{
    return make_value_node( Value{ value, location } );
}

dawn::Node dawn::make_float_node( Location const& location, Float value )
{
    return make_value_node( Value{ value, location } );
}

dawn::Node dawn::make_char_node( Location const& location, Char value )
{
    return make_value_node( Value{ value, location } );
}

dawn::Node dawn::make_string_node( Location const& location, StringRef const& value )
{
    return make_value_node( Value{ value, location } );
}

dawn::Node dawn::make_value_node( Value const& value )
{
    Node result;
    result.emplace<Value>( value );
    return result;
}
