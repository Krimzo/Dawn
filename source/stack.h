#pragma once

#include "value.h"
#include "storage.h"


namespace dawn
{
struct Frame
{
    friend struct Stack;

    void reset( RegisterRef<Frame> const& parent );

private:
    RegisterRef<Frame> m_parent;
    LocalStorage<Value> m_local_values;

    Value& set( ID id, Value const& value );
    Value* get( ID id, GlobalStorage<Value>& global_values );
};

struct Stack
{
    friend struct PopHandler;

    Stack( GlobalStorage<Value>& global_values );

    [[nodiscard]] PopHandler push();
    [[nodiscard]] PopHandler push_from( RegisterRef<Frame> const& parent_frame );

    RegisterRef<Frame> const& peek() const;

    Value& set( ID id, Value const& value );
    Value* get( ID id );

private:
    GlobalStorage<Value>& m_global_values;
    Vector<RegisterRef<Frame>> m_frames;

    void pop();
};

struct PopHandler
{
    friend struct Stack;

    PopHandler( PopHandler const& ) = delete;
    void operator=( PopHandler const& ) = delete;

    ~PopHandler() noexcept;

private:
    Stack& m_stack;

    explicit PopHandler( Stack& stack );
};
}
