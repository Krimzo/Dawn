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

    ArrayVal args_val;
    for ( int i = 2; i < argc; i++ )
        args_val.data.emplace_back( StringRef{ argv[i] } );

    Array<Node> args_node;
    args_node.emplace_back( make_value_node( args_val ) );

    ValueRef retval;
    if ( auto error = dawn.call_func( "main", args_node, retval ) )
    {
        print( error.value() );
        return -3;
    }

    return (int) retval.value().to_int();
}
