#include "nodes.h"


dawn::Node dawn::make_nothing_node()
{
    return make_value_node( ValueRef{ Value{} } );
}

dawn::Node dawn::make_bool_node( Bool value )
{
    return make_value_node( ValueRef{ value } );
}

dawn::Node dawn::make_int_node( Int value )
{
    return make_value_node( ValueRef{ value } );
}

dawn::Node dawn::make_float_node( Float value )
{
    return make_value_node( ValueRef{ value } );
}

dawn::Node dawn::make_char_node( Char value )
{
    return make_value_node( ValueRef{ value } );
}

dawn::Node dawn::make_string_node( String value )
{
    return make_value_node( ValueRef{ std::move( value ) } );
}

dawn::Node dawn::make_value_node( ValueRef const& value )
{
    Node result;
    result.emplace<RefNod>().value_ref = value;
    return result;
}
