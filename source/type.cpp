#include "type.h"
#include "node.h"
#include "syntax.h"


dawn::Enum::Entry* dawn::Enum::get( ID id )
{
    for ( auto& entry : entries )
    {
        if ( entry.id == id )
            return &entry;
    }
    return nullptr;
}

dawn::Bool dawn::Struct::contains( ID id ) const
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

std::ostream& dawn::operator<<( std::ostream& stream, VarKind kind )
{
    switch ( kind )
    {
    case VarKind::CONSTANT: stream << ""; break;
    case VarKind::VARIABLE: stream << vr_variable; break;
    case VarKind::REFERENCE: stream << vr_reference; break;
    }
    return stream;
}

std::ostream& dawn::operator<<( std::ostream& stream, VarType const& type )
{
    stream << IDSystem::get( type.type_id ) << type.kind;
    return stream;
}
