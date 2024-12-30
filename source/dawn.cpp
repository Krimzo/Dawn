#include "dawn.h"


dawn::Opt<dawn::String> dawn::Dawn::eval( StringRef const& source ) noexcept
{
    Set<String> imports;
    return eval( source, std::nullopt, imports );
}

dawn::Opt<dawn::String> dawn::Dawn::eval( StringRef const& source, Opt<String> const& parent_path, Set<String>& imported ) noexcept
{
    try
    {
        Vector<Token> tokens;
        lexer.tokenize( source, tokens );

        Module module;
        parser.parse( tokens, module );

        for ( auto& import_path : module.imports )
        {
            String path = import_path;
            if ( !fs::path( path ).is_absolute() && parent_path )
                path = *parent_path + "/" + path;

            if ( auto error = eval_file( path, imported ) )
                return error;
        }
        engine.load_mod( module );
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

dawn::Opt<dawn::String> dawn::Dawn::eval_file( StringRef const& path, Set<String>& imported ) noexcept
{
    String abs_path = fs::absolute( path ).string();
    if ( imported.contains( abs_path ) )
        return std::nullopt;

    imported.insert( abs_path );
    String parent_path = fs::path( abs_path ).parent_path().string();

    auto source = read_file( abs_path );
    if ( !source )
        return dawn::format( "file [", abs_path, "] could not be opened" );

    return eval( *source, parent_path, imported );
}

void dawn::Dawn::bind_func( String const& name, Function::CppFunc cpp_func ) noexcept
{
    engine.bind_func( IDSystem::get( name ), cpp_func );
}

dawn::Opt<dawn::String> dawn::Dawn::call_func( String const& name ) noexcept
{
    std::initializer_list<ValueRef> args;
    ValueRef retval;
    return call_func( name, args, retval );
}

dawn::Opt<dawn::String> dawn::Dawn::call_func( String const& name, ValueRef& retval ) noexcept
{
    std::initializer_list<ValueRef> args;
    return call_func( name, args, retval );
}

dawn::Opt<dawn::String> dawn::Dawn::call_func( String const& name, std::initializer_list<ValueRef> const& args ) noexcept
{
    ValueRef retval;
    return call_func( name, args, retval );
}

dawn::Opt<dawn::String> dawn::Dawn::call_func( String const& name, std::initializer_list<ValueRef> const& args, ValueRef& retval ) noexcept
{
    try
    {
        retval = engine.call_func( IDSystem::get( name ), args.begin(), (Int) args.size() );
    }
    catch ( String const& msg )
    {
        return msg;
    }
    catch ( ValueRef const& err )
    {
        return dawn::format( "Uncaught error: ", err.to_string( engine ) );
    }
    return std::nullopt;
}

void dawn::Dawn::add_var( VariableKind kind, String const& name, ValueRef const& value ) noexcept
{
    engine.add_var( kind, IDSystem::get( name ), value );
}

dawn::ValueRef* dawn::Dawn::get_var( String const& name ) noexcept
{
    return engine.get_var( IDSystem::get( name ) );
}
