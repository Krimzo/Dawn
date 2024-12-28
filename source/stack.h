#pragma once

#include "value.h"


namespace dawn
{
struct StackHelper;

struct ScopeObject
{
    enum struct ViewType
    {
        LOCAL = 0,
        GLOBAL = 1,
    };

    using LocalType = Vector<Pair<Int, ValueRef>>;
    using GlobalType = Vector<ValueRef>;

    ScopeObject( ViewType type = ViewType::LOCAL );

    ValueRef& set( Int id, ValueRef const& value );
    ValueRef* get( Int id );

    void reset( RegisterRef<ScopeObject> const& parent );

private:
    RegisterRef<ScopeObject> m_parent;
    Variant<LocalType, GlobalType> m_objects;
};

struct ScopeStack
{
    ScopeStack();

    [[nodiscard]] StackHelper push();
    [[nodiscard]] StackHelper push( Function const& func );
    void pop();

    ScopeObject& root();
    ScopeObject& current();

    RegisterRef<ScopeObject> const& peek() const;

private:
    Vector<RegisterRef<ScopeObject>> m_scopes;
};

struct StackHelper
{
    friend struct ScopeStack;

    ScopeStack& stack;

    StackHelper( ScopeStack const& ) = delete;
    void operator=( ScopeStack const& ) = delete;

    ~StackHelper() noexcept;

private:
    explicit StackHelper( ScopeStack& stack );
};
}
