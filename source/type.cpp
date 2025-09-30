#include "type.h"
#include "node.h"
#include "syntax.h"


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

std::ostream& dawn::operator<<( std::ostream& stream, VarType::Kind kind )
{
    switch ( kind )
    {
    case VarType::Kind::DEFAULT: stream << ""; break;
    case VarType::Kind::CONSTANT: stream << vr_constant; break;
    case VarType::Kind::REFERENCE: stream << vr_reference; break;
    }
    return stream;
}

std::ostream& dawn::operator<<( std::ostream& stream, VarType const& type )
{
    stream << IDSystem::get( type.type_id ) << type.kind;
    return stream;
}
