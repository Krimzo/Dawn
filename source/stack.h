#pragma once

#include "values.h"


namespace dawn
{
struct StackHelper;

struct ScopeObject
{
    ScopeObject* parent = nullptr;
    Array<Pair<Int, ValueRef>> objects;

    ScopeObject();

    ValueRef& set( Int id, ValueRef const& value );
    ValueRef* get( Int id );
};

struct ScopeStack
{
    ScopeStack( Int size );

    [[nodiscard]] StackHelper push();
    [[nodiscard]] StackHelper push( Function const& func );
    void pop();

    ScopeObject& root();
    ScopeObject& peek();

private:
    Array<ScopeObject> m_scopes;
    Int m_current = 0;
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
