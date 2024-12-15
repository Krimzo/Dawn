#include "dawn.h"


dawn::Dawn::Dawn() noexcept
{
    m_engine.load_standard();
}

dawn::Opt<dawn::String> dawn::Dawn::eval( StringRef const& source ) noexcept
{
    Set<String> imports;
    return eval( source, imports );
}

dawn::Opt<dawn::String> dawn::Dawn::eval( StringRef const& source, Set<String>& imports ) noexcept
{
    try
    {
        Array<Token> tokens;
        m_lexer.tokenize( source, tokens );

        Module module;
        m_parser.parse( tokens, module );

        for ( auto& entry : module.imports )
        {
            if ( auto error = eval_file( entry, imports ) )
                return error;
        }
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
    Set<String> imports;
    return eval_file( path, imports );
}

dawn::Opt<dawn::String> dawn::Dawn::eval_file( StringRef const& path, Set<String>& imports ) noexcept
{
    String abs_path = fs::absolute( path ).string();
    if ( imports.contains( abs_path ) )
        return std::nullopt;
    imports.insert( abs_path );

    if ( auto source = read_file( abs_path ) )
        return eval( *source, imports );
    return dawn::format( "file [", abs_path, "] could not be opened" );
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
    catch ( ValueRef const& err )
    {
        return dawn::format( "Uncaught error: ", err.value().to_string() );
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
