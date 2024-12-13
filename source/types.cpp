#include "types.h"


dawn::Variable* dawn::Struct::get_field( IDSystem& system, Int id )
{
    for ( auto& field : fields )
    {
        if ( field.name.get( system ) == id )
            return &field;
    }
    return nullptr;
}

dawn::Function* dawn::Struct::get_method( IDSystem& system, Int id )
{
    for ( auto& method : methods )
    {
        if ( method.name.get( system ) == id )
            return &method;
    }
    return nullptr;
}
