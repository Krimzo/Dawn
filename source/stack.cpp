#include "stack.h"
#include "pool.h"

static constexpr dawn::Int LOCAL_FRAME_RESERVE_SIZE = 16;
static constexpr dawn::Int GLOBAL_FRAME_SIZE = 1024;

dawn::Frame::Frame( FrameType type )
{
    if ( type == FrameType::LOCAL )
        m_frame.emplace<LocalFrame>( LOCAL_FRAME_RESERVE_SIZE );
    else
        m_frame.emplace<GlobalFrame>( GLOBAL_FRAME_SIZE );
}

dawn::Value& dawn::Frame::set( Int id, Value const& value )
{
    if ( std::holds_alternative<LocalFrame>( m_frame ) )
        return std::get<LocalFrame>( m_frame ).set( id, value );
    else
        return std::get<GlobalFrame>( m_frame ).set( id, value );
}

dawn::Value* dawn::Frame::get( Int id )
{
    if ( std::holds_alternative<LocalFrame>( m_frame ) )
    {
        if ( Value* ptr = std::get<LocalFrame>( m_frame ).get( id ) )
            return ptr;
    }
    else
    {
        if ( Value* ptr = std::get<GlobalFrame>( m_frame ).get( id ) )
            return ptr;
    }
    return m_parent.valid() ? m_parent.value().get( id ) : nullptr;
}

void dawn::Frame::reset( RegisterRef<Frame> const& parent )
{
    if ( std::holds_alternative<LocalFrame>( m_frame ) )
        std::get<LocalFrame>( m_frame ).clear();
    else
        std::get<GlobalFrame>( m_frame ).clear();
    m_parent = parent;
}

dawn::Stack::Stack()
{
    m_frames.reserve( 128 );
    m_frames.emplace_back( frame_pool().new_register() )
        .value() = Frame{ FrameType::GLOBAL };
}

dawn::PopHandler dawn::Stack::push()
{
    m_frames.emplace_back( frame_pool().new_register() )
        .value().reset( *( ++m_frames.rbegin() ) );
    return PopHandler{ *this };
}

dawn::PopHandler dawn::Stack::push_from( RegisterRef<Frame> const& frame )
{
    m_frames.emplace_back( frame_pool().new_register() )
        .value().reset( frame.valid() ? frame : m_frames.front() );
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
