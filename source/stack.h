#pragma once

#include "values.h"


namespace dawn
{
struct StackHelper;

struct ScopeObject
{
    WeakRegisterRef<ScopeObject> parent;
    Array<Pair<Int, ValueRef>> objects;

    ScopeObject();

    ValueRef& set( Int id, ValueRef const& value );
    ValueRef* get( Int id );
};

struct ScopeStack
{
    ScopeStack();

    [[nodiscard]] StackHelper push_from_root();
    [[nodiscard]] StackHelper push_from_current();
    void pop();

    ScopeObject& root();
    ScopeObject& current();

private:
    Array<RegisterRef<ScopeObject>> m_scopes;
};

struct StackHelper
{
    friend struct ScopeStack;

    ScopeStack& stack;

    ~StackHelper() noexcept;

private:
    explicit StackHelper( ScopeStack& stack );
};
}
