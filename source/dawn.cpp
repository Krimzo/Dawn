#include "dawn.h"


dawn::Opt<dawn::String> dawn::Dawn::eval( Source const& source ) noexcept
{
    StringSet imports;
    return eval( source, imports );
}

dawn::Opt<dawn::String> dawn::Dawn::eval( Source const& source, StringSet& imported ) noexcept
{
    try
    {
        if ( source.path )
        {
            if ( imported.contains( *source.path ) )
                return std::nullopt;
            imported.insert( *source.path );
        }

        Vector<Token> tokens;
        lexer.tokenize( source, tokens );

        Module module;
        parser.parse( tokens, module );
        optimizer.optimize( module );

        for ( auto& import_path : module.imports )
        {
            String path = import_path;
            if ( fs::path{ path }.is_relative() )
            {
                if ( !source.path )
                    throw String( "import can only be used inside dawn files" );
                path = fs::path{ *source.path }.parent_path().string() + "/" + path;
            }
            if ( auto error = eval( Source::from_file( path ), imported ) )
                return error;
        }
        global.load_mod( engine, module );
    }
    catch ( String const& err )
    {
        return err;
    }
    return std::nullopt;
}

void dawn::Dawn::bind_func( StringRef const& name, Bool is_ctime, CFunction cfunc ) noexcept
{
    global.bind_func( IDSystem::get( name ), is_ctime, std::move( cfunc ) );
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
            *retval = global.call_func( engine, IDSystem::get( name ), args, arg_count );
        else
            global.call_func( engine, IDSystem::get( name ), args, arg_count );
    }
    catch ( String const& err )
    {
        return err;
    }
    catch ( Value const& script_err )
    {
        return dawn::format( "Uncaught error: ", script_err.to_string( engine ) );
    }
    return std::nullopt;
}

void dawn::Dawn::add_gvar( StringRef const& name, Value const& value ) noexcept
{
    global.values.set( IDSystem::get( name ), value );
}

dawn::Value* dawn::Dawn::get_gvar( StringRef const& name ) noexcept
{
    return global.values.get( IDSystem::get( name ) );
}
