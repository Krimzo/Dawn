#pragma once

#include "parser.h"


namespace dawn
{
struct EngineError
{
    String msg;

    template<typename... Args>
    EngineError( Args const&... args )
    {
        StringStream stream;
        (stream << ... << args);
        msg = stream.str();
    }

    inline operator auto& () const
    {
        return msg;
    }
};

std::wostream& operator<<( std::wostream& stream, EngineError const& error );

struct Engine
{
    Opt<EngineError> load( Module const& module );
    Opt<EngineError> exec( String const& func_name, ListRef<Ref<Value>> const& args, Ref<Value>& retval );

private:
    Map<String, Module> m_modules;

    Opt<EngineError> handle_func( Function const& func, ListRef<Ref<Value>> const& args, Ref<Value>& retval );
};
}
