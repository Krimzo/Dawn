#include "stack.h"
#include "pool.h"


dawn::ScopeObject::ScopeObject()
{
    objects.reserve( 16 );
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
    m_scopes.push_back( scope_pool().new_register() );
}

dawn::StackHelper dawn::ScopeStack::push()
{
    auto& scope = m_scopes.emplace_back( scope_pool().new_register() ).value();
    scope.parent = *(++m_scopes.rbegin());
    scope.objects.clear();
    return StackHelper{ *this };
}

dawn::StackHelper dawn::ScopeStack::push( Function const& func )
{
    auto& scope = m_scopes.emplace_back( scope_pool().new_register() ).value();
    scope.parent = func.is_lambda() ? func.lambda_parent : m_scopes.front();
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

dawn::RegisterRef<dawn::ScopeObject> const& dawn::ScopeStack::peek() const
{
    return m_scopes.back();
}

dawn::StackHelper::~StackHelper() noexcept
{
    stack.pop();
}

dawn::StackHelper::StackHelper( ScopeStack& stack )
    : stack( stack )
{
}
