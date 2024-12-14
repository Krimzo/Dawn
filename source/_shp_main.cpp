#include "_main.h"


int dawn::_shp_main( int argc, char** argv )
{
    if ( argc < 2 )
    {
        print( "Usage: dawn <file>" );
        return -1;
    }

    Dawn dawn;
    if ( auto error = dawn.eval_file( argv[1] ) )
    {
        print( error.value() );
        return -2;
    }

    Array<Node> args;
    for ( int i = 2; i < argc; i++ )
        args.emplace_back( make_string_node( argv[i] ) );

    ValueRef retval;
    if ( auto error = dawn.call_func( "main", args, retval ) )
    {
        print( error.value() );
        return -3;
    }

    return (int) retval.value().to_int();
}
