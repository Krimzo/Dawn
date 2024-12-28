#include "stack.h"
#include "pool.h"


dawn::ScopeObject::ScopeObject( ViewType type )
{
    if ( type == ViewType::LOCAL )
    {
        auto& objects = m_objects.emplace<LocalType>();
        objects.reserve( 16 );
    }
    else
    {
        auto& objects = m_objects.emplace<GlobalType>();
        objects.resize( 1024 );
    }
}

dawn::ValueRef& dawn::ScopeObject::set( Int id, ValueRef const& value )
{
    if ( m_objects.index() == 0 )
    {
        auto& objects = *std::get_if<LocalType>( &m_objects );
        return objects.emplace_back( id, value ).second;
    }
    else
    {
        auto& objects = *std::get_if<GlobalType>( &m_objects );
        if ( (Int) objects.size() <= id )
            objects.resize( (id + 1) * 2 );
        return objects[id] = value;
    }
}

dawn::ValueRef* dawn::ScopeObject::get( Int id )
{
    if ( m_objects.index() == 0 )
    {
        auto& objects = *std::get_if<LocalType>( &m_objects );
        for ( auto& [obj_id, obj] : objects )
        {
            if ( obj_id == id )
                return &obj;
        }
    }
    else
    {
        auto& objects = *std::get_if<GlobalType>( &m_objects );
        if ( (Int) objects.size() > id )
        {
            auto& obj = objects[id];
            if ( obj.valid() )
                return &obj;
        }
    }
    return m_parent.valid() ? m_parent.value().get( id ) : nullptr;
}

void dawn::ScopeObject::reset( RegisterRef<ScopeObject> const& parent )
{
    if ( m_objects.index() == 0 )
    {
        auto& objects = *std::get_if<LocalType>( &m_objects );
        objects.clear();
    }
    else
    {
        auto& objects = *std::get_if<GlobalType>( &m_objects );
        objects.clear();
    }
    m_parent = parent;
}

dawn::ScopeStack::ScopeStack()
{
    m_scopes.reserve( 128 );
    auto& global_scope = m_scopes.emplace_back( scope_pool().new_register() );
    global_scope.value() = ScopeObject{ ScopeObject::ViewType::GLOBAL };
}

dawn::StackHelper dawn::ScopeStack::push()
{
    m_scopes.emplace_back( scope_pool().new_register() )
        .value()
        .reset( *(++m_scopes.rbegin()) );
    return StackHelper{ *this };
}

dawn::StackHelper dawn::ScopeStack::push( Function const& func )
{
    m_scopes.emplace_back( scope_pool().new_register() )
        .value()
        .reset( func.is_lambda() ? func.lambda_parent : m_scopes.front() );
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
