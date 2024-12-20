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

    if ( !parent.valid() )
        return nullptr;

    return parent.value().get( id );
}

dawn::ScopeStack::ScopeStack()
{
    m_scopes.reserve( 128 );
    m_scopes.push_back( get_global_memory().scope_memory.new_register() );
}

dawn::StackHelper dawn::ScopeStack::push_from_root()
{
    auto& scope = m_scopes.emplace_back( get_global_memory().scope_memory.new_register() ).value();
    scope.parent = m_scopes.front().as_weak();
    scope.objects.clear();
    return StackHelper{ *this };
}

dawn::StackHelper dawn::ScopeStack::push_from_current()
{
    auto& scope = m_scopes.emplace_back( get_global_memory().scope_memory.new_register() ).value();
    scope.parent = (++m_scopes.rbegin())->as_weak();
    scope.objects.clear();
    return StackHelper{ *this };
}

void dawn::ScopeStack::pop()
{
    m_scopes.pop_back();
}

dawn::ScopeObject& dawn::ScopeStack::root()
{
    return m_scopes.front().value();
}

dawn::ScopeObject& dawn::ScopeStack::current()
{
    return m_scopes.back().value();
}

dawn::StackHelper::~StackHelper() noexcept
{
    stack.pop();
}

dawn::StackHelper::StackHelper( ScopeStack& stack )
    : stack( stack )
{}
