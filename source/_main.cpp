#include "dawn.h"

using namespace dawn; // Only in this case since it is not a header file.

struct Stopwatch
{
    using Clock = ch::high_resolution_clock;
    const Clock::time_point start_time = Clock::now();

    ~Stopwatch() noexcept
    {
        const Clock::time_point end_time = Clock::now();
        const auto duration = ch::duration_cast<ch::microseconds>( end_time - start_time );
        print( "Elapsed time: ", duration );
    }
};

int _dev_main( int argc, char** argv );
int _shp_main( int argc, char** argv );

int main( int argc, char** argv )
{
#ifdef DAWN_SHIP
    return _shp_main( argc, argv );
#else
    return _dev_main( argc, argv );
#endif
}

int _dev_main( int argc, char** argv )
{
    Stopwatch stopwatch;
    Dawn dawn;

    if ( auto error = dawn.eval( Source::from_file(
#if _DEBUG
#if 0
        "examples/tests.dw"
#else
        "examples/dev.dw"
#endif
#else
        "examples/bench.dw"
#endif
    ) ) )
    {
        print( error.value() );
        return -1;
    }

    if ( auto error = dawn.call_func( "main" ) )
    {
        print( error.value() );
        return -2;
    }
    return 0;
}

int _shp_main( int argc, char** argv )
{
    if ( argc < 2 )
    {
        print( "Usage: dawn <file>" );
        return -1;
    }

    Dawn dawn;
    if ( auto error = dawn.eval( Source::from_file( argv[1] ) ) )
    {
        print( error.value() );
        return -2;
    }

    ArrayValue arg;
    for ( int i = 2; i < argc; i++ )
        arg.data.emplace_back( String{ argv[i] } );

    Value retval{ Int() };
    if ( auto error = dawn.call_func( "main", { Value{ arg } }, &retval ) )
    {
        print( error.value() );
        return -3;
    }

    Int retcode = retval.to_int( Location::none, dawn.engine );
    return static_cast<int>( retcode );
}
