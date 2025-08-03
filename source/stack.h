#pragma once

#include "value.h"


namespace dawn
{
struct Frame
{
    using LocalFrame = Vector<Pair<Int, Value>>;
    using GlobalFrame = Vector<Opt<Value>>;

    Frame( FrameType type = FrameType::LOCAL );

    Value& set( Int id, Value const& value );
    Value* get( Int id );

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
