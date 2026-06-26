#include "dawn.h"


dawn::Opt<dawn::String> dawn::Config::from_args( char const* const* args, int count ) noexcept
{
    for ( int i = 0; i < count; i++ )
    {
        const std::string_view arg = args[i];
        if ( arg.starts_with( Flags::_PREFIX ) )
        {
            if ( arg.size() < Flags::_PREFIX.size() + 1 )
                return format( "invalid argument: ", arg );
            const std::string_view arg_value = arg.substr( Flags::_PREFIX.size() );
            const auto it = flags.find( arg_value );
            if ( it == flags.end() )
                return format( "unknown flag: ", arg );
            it->second = true;
        }
        else if ( input_file.empty() )
            input_file = arg;
        else
            return format( "input file already provided, error: ", arg );
    }
    return std::nullopt;
}

dawn::Opt<dawn::String> dawn::Config::from_file( StringRef const& path ) noexcept
{
    const Opt<String> file_data = read_file( path );
    if ( !file_data )
        return format( "failed to read file: ", path );
    Bool has_main = false;
    for ( String const& line : split( *file_data, "\n" ) )
    {
        const Vector<String> parts = split( line, "=" );
        if ( parts.empty() )
            continue;
        if ( parts.size() != 2 )
            return format( "invalid line: ", line );
        if ( parts[0] == Flags::_CONFIG_MAIN_FILE )
        {
            input_file = parts[1];
            has_main = true;
        }
        else
        {
            const auto it = flags.find( parts[0] );
            if ( it == flags.end() )
                return format( "unknown config flag: ", parts[0] );
            it->second = ( parts[1] == "true" );
        }
    }
    if ( !has_main )
        return format( "missing config flag: ", Flags::_CONFIG_MAIN_FILE );
    return std::nullopt;
}

dawn::Bool dawn::Config::flag_status( StringRef const& flag ) const
{
    const auto it = flags.find( flag );
    if ( it == flags.end() )
        return false;
    return it->second;
}

dawn::Opt<dawn::String> dawn::Dawn::eval( Source const& source ) noexcept
{
    try
    {
        if ( source.path )
        {
            if ( imports.contains( *source.path ) )
                return std::nullopt;
            imports.insert( *source.path );
        }

        Vector<Token> tokens;
        lexer.tokenize( source, tokens );

        Module module;
        parser.parse( tokens, module );

        for ( String path : module.imports )
        {
            if ( fs::path{ path }.is_relative() )
            {
                if ( !source.path )
                    throw String( "import can only be used inside dawn files" );
                path = fs::path{ *source.path }.parent_path().string() + "/" + path;
            }
            if ( auto error = eval( Source::from_file( path ) ) )
                return error;
        }

        if ( !config.flag_status( Flags::DISABLE_OPTIMIZATIONS ) )
            optimizer.optimize( module );
        engine.load_mod( module );
    }
    catch ( String const& msg )
    {
        return msg;
    }
    return std::nullopt;
}

void dawn::Dawn::bind_func( StringRef const& name, Bool is_ctime, CFunction cfunc ) noexcept
{
    engine.bind_func( IDSystem::get( name ), is_ctime, std::move( cfunc ) );
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
        return dawn::format( "Uncaught error: ", err.to_string( engine ) );
    }
    return std::nullopt;
}

void dawn::Dawn::add_var( Location const& location, VarType const& type, StringRef const& name, Value const& value ) noexcept
{
    engine.add_var( location, type, IDSystem::get( name ), value );
}

dawn::Value* dawn::Dawn::get_var( StringRef const& name ) noexcept
{
    return engine.get_var( IDSystem::get( name ) );
}
