#include "stack.h"
#include "pool.h"


dawn::Frame::Frame( FrameType type )
{
    if ( type == FrameType::LOCAL )
    {
        auto& frame = m_frame.emplace<LocalFrame>();
        frame.reserve( 16 );
    }
    else
    {
        auto& frame = m_frame.emplace<GlobalFrame>();
        frame.resize( 1024 );
    }
}

dawn::ValueRef& dawn::Frame::set( Int id, ValueRef const& value )
{
    if ( m_frame.index() == 0 )
    {
        auto& frame = std::get<LocalFrame>( m_frame );
        return frame.emplace_back( id, value ).second;
    }
    else
    {
        auto& frame = std::get<GlobalFrame>( m_frame );
        if ( (Int) frame.size() <= id )
            frame.resize( (id + 1) * 2 );
        return frame[id] = value;
    }
}

dawn::ValueRef* dawn::Frame::get( Int id )
{
    if ( m_frame.index() == 0 )
    {
        auto& frame = std::get<LocalFrame>( m_frame );
        for ( auto& [obj_id, obj] : frame )
        {
            if ( obj_id == id )
                return &obj;
        }
    }
    else
    {
        auto& frame = std::get<GlobalFrame>( m_frame );
        if ( (Int) frame.size() > id )
        {
            auto& obj = frame[id];
            if ( obj.valid() )
                return &obj;
        }
    }
    return m_parent.valid() ? m_parent.value().get( id ) : nullptr;
}

void dawn::Frame::reset( RegisterRef<Frame> const& parent )
{
    if ( m_frame.index() == 0 )
    {
        auto& frame = std::get<LocalFrame>( m_frame );
        frame.clear();
    }
    else
    {
        auto& frame = std::get<GlobalFrame>( m_frame );
        frame.clear();
    }
    m_parent = parent;
}

dawn::Stack::Stack()
{
    m_frames.reserve( 128 );
    auto& global_frame = m_frames.emplace_back( frame_pool().new_register() );
    global_frame.value() = Frame{ FrameType::GLOBAL };
}

dawn::PopHandler dawn::Stack::push()
{
    m_frames.emplace_back( frame_pool().new_register() )
        .value()
        .reset( *(++m_frames.rbegin()) );
    return PopHandler{ *this };
}

dawn::PopHandler dawn::Stack::push_from( RegisterRef<Frame> const& frame )
{
    m_frames.emplace_back( frame_pool().new_register() )
        .value()
        .reset( frame.valid() ? frame : m_frames.front() );
    return PopHandler{ *this };
}

void dawn::Stack::pop()
{
    m_frames.pop_back();
}

dawn::Frame& dawn::Stack::root()
{
    return m_frames.front().value();
}

dawn::Frame& dawn::Stack::current()
{
    return m_frames.back().value();
}

dawn::RegisterRef<dawn::Frame> const& dawn::Stack::peek() const
{
    return m_frames.back();
}

dawn::PopHandler::~PopHandler() noexcept
{
    stack.pop();
}

dawn::PopHandler::PopHandler( Stack& stack )
    : stack( stack )
{
}
