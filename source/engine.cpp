#include "engine.h"


std::wostream& dawn::operator<<( std::wostream& stream, EngineError const& error )
{
    stream << error.msg;
    return stream;
}

dawn::Opt<dawn::EngineError> dawn::Engine::load( Module const& module )
{
    if ( m_modules.contains( module.name ) )
        return EngineError{ L"module [", module.name, L"] already loaded" };

    Set<String> keys;
    keys.insert( std::views::keys( module.space_public.variables ).begin(), std::views::keys( module.space_public.variables ).end() );
    keys.insert( std::views::keys( module.space_public.functions ).begin(), std::views::keys( module.space_public.functions ).end() );
    keys.insert( std::views::keys( module.space_public.enums ).begin(), std::views::keys( module.space_public.enums ).end() );
    keys.insert( std::views::keys( module.space_public.layers ).begin(), std::views::keys( module.space_public.layers ).end() );
    keys.insert( std::views::keys( module.space_public.structs ).begin(), std::views::keys( module.space_public.structs ).end() );

    for ( auto& key : keys )
    {
        for ( auto& [_, mod] : m_modules )
        {
            if ( mod.contains_id( key ) )
                return EngineError{ L"[", key, L"] already exists" };
        }
    }

    m_modules[module.name] = module;

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::exec( String const& func_name, ListRef<Ref<Value>> const& args, Ref<Value>& retval )
{
    Function* func = nullptr;
    for ( auto& [_, mod] : m_modules )
    {
        auto& functions = mod.space_public.functions;
        if ( functions.contains( func_name ) )
        {
            func = &functions.at( func_name );
            break;
        }
    }

    if ( !func )
        return EngineError{ L"function doesn't exist" };

    if ( auto error = handle_func( *func, args, retval ) )
        return error;

    return std::nullopt;
}

dawn::Opt<dawn::EngineError> dawn::Engine::handle_func( Function const& func, ListRef<Ref<Value>> const& args, Ref<Value>& retval )
{
    if ( func.args.size() != args.size() )
        return EngineError{ "invalid argument count" };

    assert( false && "not impl" );
    return std::nullopt;
}
