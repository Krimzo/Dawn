#include "type.h"
#include "node.h"


dawn::Bool dawn::Function::is_lambda() const
{
    return id <= 0;
}

dawn::Bool dawn::Function::is_method() const
{
    return self->type() != ValueType::NOTHING;
}

dawn::Bool dawn::Struct::contains( Int id ) const
{
    for ( auto& field : fields )
    {
        if ( field.id == id )
            return true;
    }
    for ( auto& method : methods )
    {
        if ( method.id == id )
            return true;
    }
    return false;
}
