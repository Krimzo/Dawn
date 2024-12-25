#pragma once

#include "value.h"


namespace dawn
{
struct StackHelper;

struct ScopeObject
{
    RegisterRef<ScopeObject> parent;
    Vector<Pair<Int, ValueRef>> objects;

    ScopeObject();

    ValueRef& set( Int id, ValueRef const& value );
    ValueRef* get( Int id );
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

    StackHelper( ScopeStack&& ) = delete;
    void operator=( ScopeStack&& ) = delete;

    ~StackHelper() noexcept;

private:
    explicit StackHelper( ScopeStack& stack );
};
}
