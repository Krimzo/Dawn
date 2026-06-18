#include "dawn.h"

using namespace dawn; // Only in this case since it is not a header file.

#define DEBUG_TESTS 0

#ifndef DAWN_SHIP

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

int main( int argc, char** argv )
{
    Stopwatch stopwatch;
    Dawn dawn;

    if ( auto error = dawn.eval( Source::from_file(
#if _DEBUG
#if DEBUG_TESTS
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

#else

int main( int argc, char** argv )
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
        arg.data.emplace_back( String{ argv[i] }, LOCATION_NONE );

    Value retval{ Int(), LOCATION_NONE };
    if ( auto error = dawn.call_func( "main", { Value{ arg, LOCATION_NONE } }, &retval ) )
    {
        print( error.value() );
        return -3;
    }
    return (int) retval.to_int( dawn.engine );
}

#endif
