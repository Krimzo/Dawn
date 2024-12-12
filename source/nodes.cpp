#include "nodes.h"


dawn::Node dawn::make_nothing_node()
{
    return make_value_node( Value{} );
}

dawn::Node dawn::make_bool_node( Bool value )
{
    return make_value_node( Value{ value } );
}

dawn::Node dawn::make_int_node( Int value )
{
    return make_value_node( Value{ value } );
}

dawn::Node dawn::make_float_node( Float value )
{
    return make_value_node( Value{ value } );
}

dawn::Node dawn::make_char_node( Char value )
{
    return make_value_node( Value{ value } );
}

dawn::Node dawn::make_string_node( StringRef const& value )
{
    return make_value_node( Value{ value } );
}

dawn::Node dawn::make_value_node( Value const& value )
{
    Node result;
    result.store<RefNod>().value_ref = ValueRef{ value };
    return result;
}
