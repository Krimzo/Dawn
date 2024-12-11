#include "dawn.h"


dawn::Dawn::Dawn() noexcept
{
    m_engine.load_default_mods();
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

void dawn::Dawn::bind_func( String const& name, Function::CppFunc cpp_func ) noexcept
{
    m_engine.bind_func( name, cpp_func );
}

dawn::Opt<dawn::String> dawn::Dawn::call_func( String const& name, Array<Node> const& args, ValueRef& retval ) noexcept
{
    try
    {
        m_engine.call_func( name, args, retval );
    }
    catch ( String const& msg )
    {
        return msg;
    }
    return std::nullopt;
}

dawn::Opt<dawn::String> dawn::Dawn::add_var( Variable const& var ) noexcept
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

void dawn::Dawn::add_var( Variable const& var, ValueRef const& value ) noexcept
{
    m_engine.add_var( var, value );
}

dawn::ValueRef* dawn::Dawn::get_var( String const& name ) noexcept
{
    return m_engine.get_var( name );
}
