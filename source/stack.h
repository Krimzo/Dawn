#pragma once

#include "value.h"
#include "storage.h"


namespace dawn
{
struct StackItem
{
    ID id;
    Value value;
};

struct Stack
{
    Stack();

    [[nodiscard]] PopHandler mark_frame();
    void push( ID id, Value const& value );
    Value* get( ID id );

private:
    Vector<StackItem> m_items;
};

struct PopHandler
{
    friend struct Stack;

    Vector<StackItem>& stack_data;
    const size_t stack_size;

    PopHandler( PopHandler const& ) = delete;
    void operator=( PopHandler const& ) = delete;

    ~PopHandler() noexcept;

private:
    explicit PopHandler( Vector<StackItem>& stack_data );
};
}
