#include "dawn.h"


dawn::Opt<dawn::String> dawn::Dawn::eval( Source const& source ) noexcept
{
    Set<String> imports;
    return eval( source, imports );
}

dawn::Opt<dawn::String> dawn::Dawn::eval( Source const& source, Set<String>& imported ) noexcept
{
    try
    {
        if ( !source.path.empty() )
        {
            if ( imported.contains( source.path ) )
                return std::nullopt;
            imported.insert( source.path );
        }

        Vector<Token> tokens;
        lexer.tokenize( source, tokens );

        Module module;
        parser.parse( tokens, module );

        for ( auto& import_path : module.imports )
        {
            String path = import_path;
            if ( !fs::path{ path }.is_absolute() )
            {
                if ( source.path.empty() )
                    throw String( "import can only be used inside dawn files" );
                path = fs::path{ source.path }.parent_path().string() + "/" + path;
            }
            if ( auto error = eval( Source::from_file( path ), imported ) )
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

void dawn::Dawn::bind_func( StringRef const& name, CFunction cfunc ) noexcept
{
    engine.bind_cfunc( IDSystem::get( name ), std::move( cfunc ) );
}

dawn::Opt<dawn::String> dawn::Dawn::call_func( StringRef const& name ) noexcept
{
    return call_func( name, nullptr, 0, nullptr );
}

dawn::Opt<dawn::String> dawn::Dawn::call_func( StringRef const& name, Value* retval ) noexcept
{
    return call_func( name, nullptr, 0, retval );
}

dawn::Opt<dawn::String> dawn::Dawn::call_func( StringRef const& name, std::initializer_list<Value> const& args, Value* retval ) noexcept
{
    return call_func( name, (Value*) args.begin(), (Int) args.size(), retval );
}

dawn::Opt<dawn::String> dawn::Dawn::call_func( StringRef const& name, Value* args, Int arg_count, Value* retval ) noexcept
{
    try
    {
        if ( retval )
            *retval = engine.call_func( IDSystem::get( name ), args, arg_count );
        else
            engine.call_func( IDSystem::get( name ), args, arg_count );
    }
    catch ( String const& msg )
    {
        return msg;
    }
    catch ( Value const& err )
    {
        return dawn::format( "Uncaught error: ", err.to_string( engine, Location{ Bad{} } ) );
    }
    return std::nullopt;
}

void dawn::Dawn::add_var( VariableKind kind, StringRef const& name, Value const& value ) noexcept
{
    engine.add_var( kind, IDSystem::get( name ), value );
}

dawn::Value* dawn::Dawn::get_var( StringRef const& name ) noexcept
{
    return engine.get_var( IDSystem::get( name ) );
}
