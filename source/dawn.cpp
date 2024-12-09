#include "dawn.h"


dawn::Dawn::Dawn()
{
    m_engine.load_default_mods();
}

dawn::Opt<dawn::String> dawn::Dawn::eval( StringRef const& source )
{
    Array<Token> tokens;
    if ( auto error = m_lexer.tokenize( source, tokens ) )
        return error->msg;

    Module module;
    if ( auto error = m_parser.parse( tokens, module ) )
        return error->msg;

    if ( auto error = m_engine.load_mod( module ) )
        return error->msg;

    return std::nullopt;
}

dawn::Opt<dawn::String> dawn::Dawn::eval_file( StringRef const& path )
{
    auto source = read_file( path );
    if ( !source )
        return format( "file [", path, "] could not be opened" );
    return eval( *source );
}

void dawn::Dawn::bind_func( String const& name, Function::CppFunc cpp_func )
{
    m_engine.bind_func( name, cpp_func );
}

dawn::Opt<dawn::EngineError> dawn::Dawn::call_func( String const& name, Array<Ref<Node>> const& args, ValueBox& retval )
{
    return m_engine.call_func( name, args, retval );
}

void dawn::Dawn::add_var( String const& name, Bool is_var, RawValue const& value )
{
    m_engine.add_var( name, is_var, value );
}

dawn::ValueBox* dawn::Dawn::get_var( String const& name )
{
    return m_engine.get_var( name );
}
