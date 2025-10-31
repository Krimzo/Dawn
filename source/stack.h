#pragma once

#include "value.h"
#include "storage.h"


namespace dawn
{
using Globals = GlobalStorage<Value>;
using Locals = LocalStorage<Value>;

struct Frame
{
    friend struct Stack;

    void reset( RegisterRef<Frame> const& parent );

private:
    RegisterRef<Frame> m_parent;
    Locals m_locals;

    Value& set( ID id, Value const& value );
    Value* get( ID id, Globals& globals );
};

struct Stack
{
    friend struct PopHandler;

    Stack( Globals& globals );

    [[nodiscard]] PopHandler push();
    [[nodiscard]] PopHandler push_from( RegisterRef<Frame> const& parent_frame );

    RegisterRef<Frame> const& peek() const;

    Value& set( ID id, Value const& value );
    Value* get( ID id );

private:
    Globals& m_globals;
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
