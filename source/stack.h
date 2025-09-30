#pragma once

#include "value.h"
#include "storage.h"


namespace dawn
{
struct Frame
{
    using LocalFrame = LocalStorage<Value>;
    using GlobalFrame = GlobalStorage<Value>;

    Frame( FrameType type = FrameType::LOCAL );

    Value& set( ID id, Value const& value );
    Value* get( ID id );

    void reset( RegisterRef<Frame> const& parent );

private:
    RegisterRef<Frame> m_parent;
    Variant<LocalFrame, GlobalFrame> m_frame;
};

struct Stack
{
    Stack();

    [[nodiscard]] PopHandler push();
    [[nodiscard]] PopHandler push_from( RegisterRef<Frame> const& frame );
    void pop();

    Frame& root();
    Frame& current();

    RegisterRef<Frame> const& peek() const;

private:
    Vector<RegisterRef<Frame>> m_frames;
};

struct PopHandler
{
    friend struct Stack;

    Stack& stack;

    PopHandler( PopHandler const& ) = delete;
    void operator=( PopHandler const& ) = delete;

    ~PopHandler() noexcept;

private:
    explicit PopHandler( Stack& stack );
};
}
