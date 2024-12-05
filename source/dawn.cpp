#include "util.h"
#include "lexer.h"
#include "parser.h"
#include "engine.h"


int main()
{
    //// PREP
    using namespace dawn;
    String source = read_file( L"examples/min_example.dw" );

    //// LEXER
    Lexer lexer;
    Array<Token> tokens;
    if ( auto error = lexer.tokenize( source, tokens ) )
    {
        print( error.value() );
        return 1;
    }

    if /* PRINT TOKENS */ constexpr ( 0 )
    {
        for ( auto& token : tokens )
            print( token );
    }

    //// PARSER
    Parser parser;
    Module module;
    if ( auto error = parser.parse( tokens, module ) )
    {
        print( error.value() );
        return 2;
    }

    //// ENGINE
    Engine engine;
    engine.load_default_mods();

    if ( auto error = engine.load_mod( module ) )
    {
        print( error.value() );
        return 3;
    }

    if /* GET VALUE */ constexpr ( 0 )
    {
        Ref<Value> a_val = engine.get_var( L"a" );
        Ref<Value> b_val = engine.get_var( L"b" );
        if ( !a_val || !b_val )
            return 4;

        print( L"a = ", a_val->to_string() );
        print( L"b = ", b_val->to_string() );
    }

    if /* CALL FUNCTION */ constexpr ( 1 )
    {
        Ref<Value> retval;
        if ( auto error = engine.call_func( L"main", {}, retval ) )
        {
            print( error.value() );
            return 5;
        }
    }

    return 0;
}
