#include "stack.h"
#include "pool.h"

static constexpr auto STACK_RESERVE_SIZE = 512;

dawn::Stack::Stack()
{
    m_items.reserve( STACK_RESERVE_SIZE );
}

dawn::PopHandler dawn::Stack::mark_frame()
{
    return PopHandler{ m_items };
}

void dawn::Stack::push( ID id, Value const& value )
{
    m_items.emplace_back( id, value );
}

void dawn::Stack::pop()
{
    m_items.pop_back();
}

dawn::Int dawn::Stack::count() const
{
    return (Int) m_items.size();
}

dawn::Value* dawn::Stack::get( ID id )
{
    for ( Int i = (Int) m_items.size() - 1; i >= 0; --i )
    {
        if ( m_items[i].id == id )
            return &m_items[i].value;
    }
    return nullptr;
}

dawn::PopHandler::~PopHandler() noexcept
{
    for ( size_t i = 0; i < stack_data.size() - stack_size; i++ )
        stack_data.pop_back();
}

dawn::PopHandler::PopHandler( Vector<StackItem>& stack_data )
    : stack_data( stack_data )
    , stack_size( stack_data.size() )
{}
