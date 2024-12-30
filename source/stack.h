#pragma once

#include "value.h"


namespace dawn
{
struct PopHandler;

enum struct FrameType
{
    LOCAL = 0,
    GLOBAL = 1,
};

struct Frame
{
    using LocalFrame = Vector<Pair<Int, ValueRef>>;
    using GlobalFrame = Vector<ValueRef>;

    Frame( FrameType type = FrameType::LOCAL );

    ValueRef& set( Int id, ValueRef const& value );
    ValueRef* get( Int id );

    void reset( RegisterRef<Frame> const& parent );

private:
    RegisterRef<Frame> m_parent;
    Variant<LocalFrame, GlobalFrame> m_frame;
};

struct Stack
{
    Stack();

    [[nodiscard]] PopHandler push();
    [[nodiscard]] PopHandler push( Function const& func );
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
