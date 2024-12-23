#include "stack.h"


dawn::ScopeObject::ScopeObject()
{
    objects.reserve( 8 );
}

dawn::ValueRef& dawn::ScopeObject::set( Int id, ValueRef const& value )
{
    return objects.emplace_back( id, value ).second;
}

dawn::ValueRef* dawn::ScopeObject::get( Int id )
{
    for ( auto& [obj_id, obj] : objects )
    {
        if ( obj_id == id )
            return &obj;
    }
    return parent ? parent->get( id ) : nullptr;
}

dawn::ScopeStack::ScopeStack( Int size )
{
    m_scopes.resize( size );
}

dawn::StackHelper dawn::ScopeStack::push()
{
    if ( ++m_current == m_scopes.size() )
        PANIC( "stack overflow" );

    auto& scope = peek();
    scope.parent = &m_scopes[m_current - 1];
    scope.objects.clear();
    return StackHelper{ *this };
}

dawn::StackHelper dawn::ScopeStack::push( Function const& func )
{
    if ( ++m_current == m_scopes.size() )
        PANIC( "stack overflow" );

    auto& scope = peek();
    scope.parent = &root();
    scope.objects.clear();
    return StackHelper{ *this };
}

void dawn::ScopeStack::pop()
{
    --m_current;
}

dawn::ScopeObject& dawn::ScopeStack::root()
{
    return m_scopes[0];
}

dawn::ScopeObject& dawn::ScopeStack::peek()
{
    return m_scopes[m_current];
}

dawn::StackHelper::~StackHelper() noexcept
{
    stack.pop();
}

dawn::StackHelper::StackHelper( ScopeStack& stack )
    : stack( stack )
{}
