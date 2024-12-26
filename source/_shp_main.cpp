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

    ArrayVal arg;
    for ( int i = 2; i < argc; i++ )
        arg.data.emplace_back( String{ argv[i] } );

    ValueRef retval{ 0ll };
    if ( auto error = dawn.call_func( "main", { ValueRef{ arg } }, retval ) )
    {
        print( error.value() );
        return -3;
    }

    Int retcode = retval.to_int( dawn.engine );
    return static_cast<int>(retcode);
}
