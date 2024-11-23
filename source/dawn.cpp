#include "util.h"
#include "lexer.h"
#include "parser.h"
#include "engine.h"


int main()
{
    using namespace dawn;

    String source = read_file( L"examples/min_example.dw" );

    Lexer lexer;
    Parser parser;
    Engine engine;

    Array<Token> tokens;
    if ( auto error = lexer.tokenize( source, tokens ) )
    {
        print( error.value() );
        return 1;
    }

    if constexpr ( false )
        for ( auto& token : tokens )
            print( token );

    Module module;
    if ( auto error = parser.parse( tokens, module ) )
    {
        print( error.value() );
        return 2;
    }

    if ( auto error = engine.load( module ) )
    {
        print( error.value() );
        return 3;
    }

    Ref<Value> retval;
    if ( auto error = engine.exec( L"main", { ArrayValue::make() }, retval ) )
    {
        print( error.value() );
        return 4;
    }

    return 0;
}
