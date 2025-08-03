#include "type.h"
#include "node.h"
#include "syntax.h"


dawn::FunctionType dawn::Function::type() const
{
    if ( id <= 0 )
        return FunctionType::LAMBDA;
    else if ( METHOD_self->type() != ValueType::NOTHING )
        return FunctionType::METHOD;
    else
        return FunctionType::GLOBAL;
}

dawn::Bool dawn::Enum::contains( Int id ) const
{
    for ( auto& entry : entries )
    {
        if ( entry.id == id )
            return true;
    }
    return false;
}

dawn::Enum::Entry* dawn::Enum::get( Int id )
{
    for ( auto& entry : entries )
    {
        if ( entry.id == id )
            return &entry;
    }
    return nullptr;
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
