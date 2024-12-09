#include "types.h"


dawn::Function const* dawn::Struct::get_method( StringRef const& name ) const
{
    for ( auto& method : methods )
    {
        if ( method.name == name )
            return &method;
    }
    return nullptr;
}
