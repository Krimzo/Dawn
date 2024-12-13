#include "dawn.h"


dawn::Dawn::Dawn() noexcept
{
    m_engine.load_standard();
}

dawn::Opt<dawn::String> dawn::Dawn::eval( StringRef const& source ) noexcept
{
    try
    {
        Array<Token> tokens;
        Module module;
        m_lexer.tokenize( source, tokens );
        m_parser.parse( tokens, module );
        m_engine.load_mod( module );
    }
    catch ( String const& msg )
    {
        return msg;
    }
    return std::nullopt;
}

dawn::Opt<dawn::String> dawn::Dawn::eval_file( StringRef const& path ) noexcept
{
    auto source = read_file( path );
    if ( !source )
        return format( "file [", path, "] could not be opened" );
    return eval( *source );
}

void dawn::Dawn::bind_func( StringRef const& name, Function::CppFunc cpp_func ) noexcept
{
    m_engine.bind_func( name, cpp_func );
}

dawn::Opt<dawn::String> dawn::Dawn::call_func( String const& name ) noexcept
{
    Array<Node> args;
    ValueRef retval;
    return call_func( name, args, retval );
}

dawn::Opt<dawn::String> dawn::Dawn::call_func( String const& name, ValueRef& retval ) noexcept
{
    Array<Node> args;
    return call_func( name, args, retval );
}

dawn::Opt<dawn::String> dawn::Dawn::call_func( String const& name, Array<Node>& args ) noexcept
{
    ValueRef retval;
    return call_func( name, args, retval );
}

dawn::Opt<dawn::String> dawn::Dawn::call_func( String const& name, Array<Node>& args, ValueRef& retval ) noexcept
{
    try
    {
        m_engine.call_func( m_engine.id_system.get( name ), args, retval );
    }
    catch ( String const& msg )
    {
        return msg;
    }
    return std::nullopt;
}

dawn::Opt<dawn::String> dawn::Dawn::add_var( Variable& var ) noexcept
{
    try
    {
        m_engine.add_var( var );
    }
    catch ( String const& msg )
    {
        return msg;
    }
    return std::nullopt;
}

void dawn::Dawn::add_var( VariableKind kind, String const& name, ValueRef const& value ) noexcept
{
    m_engine.add_var( kind, m_engine.id_system.get( name ), value );
}

dawn::ValueRef* dawn::Dawn::get_var( String const& name ) noexcept
{
    return m_engine.get_var( m_engine.id_system.get( name ) );
}
