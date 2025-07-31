#include "type.h"
#include "node.h"
#include "syntax.h"


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

std::ostream& dawn::operator<<( std::ostream& stream, VariableKind kind )
{
    switch ( kind )
    {
    case VariableKind::VAR: stream << kw_var; break;
    case VariableKind::REF: stream << kw_ref; break;
    }
    return stream;
}
