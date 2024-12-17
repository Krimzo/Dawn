#include "types.h"


dawn::Bool dawn::Operator::is_unary() const
{
    return args.size() == 1;
}

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

dawn::Operator* dawn::Struct::get_operator( IDSystem& system, Int id, Bool is_unary )
{
    for ( auto& op : operators )
    {
        if ( op.name.get( system ) == id && op.is_unary() == is_unary )
            return &op;
    }
    return nullptr;
}
