#include "stack.h"
#include "pool.h"


void dawn::Frame::reset( RegisterRef<Frame> const& parent )
{
    m_parent = parent;
    m_local_values.clear();
}

dawn::Value& dawn::Frame::set( ID id, Value const& value )
{
    return m_local_values.set( id, value );
}

dawn::Value* dawn::Frame::get( ID id, GlobalStorage<Value>& global_values )
{
    if ( Value* ptr = m_local_values.get( id ) )
        return ptr;
    else if ( m_parent )
        return m_parent->get( id, global_values );
    else
        return global_values.get( id );
}

dawn::Stack::Stack( GlobalStorage<Value>& global_values )
    : m_global_values( global_values )
{
    m_frames.reserve( 64 );
}

dawn::PopHandler dawn::Stack::push()
{
    m_frames.emplace_back( frame_pool().new_register() )
        ->reset( *( ++m_frames.rbegin() ) );
    return PopHandler{ *this };
}

dawn::PopHandler dawn::Stack::push_from( RegisterRef<Frame> const& parent_frame )
{
    m_frames.emplace_back( frame_pool().new_register() )
        ->reset( parent_frame );
    return PopHandler{ *this };
}

dawn::RegisterRef<dawn::Frame> const& dawn::Stack::peek() const
{
    if ( !m_frames.empty() )
        return m_frames.back();
    else
    {
        static const RegisterRef<Frame> empty{};
        return empty;
    }
}

dawn::Value& dawn::Stack::set( ID id, Value const& value )
{
    if ( m_frames.empty() )
        return m_global_values.set( id, value );
    else
        return m_frames.back()->set( id, value );
}

dawn::Value* dawn::Stack::get( ID id )
{
    if ( m_frames.empty() )
        return m_global_values.get( id );
    else
        return m_frames.back()->get( id, m_global_values );
}

void dawn::Stack::pop()
{
    m_frames.pop_back();
}

dawn::PopHandler::~PopHandler() noexcept
{
    m_stack.pop();
}

dawn::PopHandler::PopHandler( Stack& stack )
    : m_stack( stack )
{
}
