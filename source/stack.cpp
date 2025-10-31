#include "stack.h"
#include "pool.h"


void dawn::Frame::reset( RegisterRef<Frame> const& parent )
{
    m_parent = parent;
    m_locals.clear();
}

dawn::Value& dawn::Frame::set( ID id, Value const& value )
{
    return m_locals.set( id, value );
}

dawn::Value* dawn::Frame::get( ID id, Globals& globals )
{
    if ( Value* ptr = m_locals.get( id ) )
        return ptr;
    else if ( m_parent )
        return m_parent->get( id, globals );
    else
        return globals.get( id );
}

dawn::Stack::Stack( Globals& globals )
    : m_globals( globals )
{
    m_frames.reserve( 128 );
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
        ->reset( parent_frame ? parent_frame : RegisterRef<Frame>{} );
    return PopHandler{ *this };
}

dawn::RegisterRef<dawn::Frame> const& dawn::Stack::peek() const
{
    return m_frames.back();
}

dawn::Value& dawn::Stack::set( ID id, Value const& value )
{
    if ( m_frames.empty() )
        return m_globals.set( id, value );
    else
        return m_frames.back()->set( id, value );
}

dawn::Value* dawn::Stack::get( ID id )
{
    if ( m_frames.empty() )
        return m_globals.get( id );
    else
        return m_frames.back()->get( id, m_globals );
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
