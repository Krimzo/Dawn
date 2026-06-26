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

int main()
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
    Dawn dawn;
    if ( auto error = dawn.config.from_args( argv + 1, argc - 1 ) )
    {
        print( error.value() );
        return -1;
    }

    try {
        const Source source = Source::from_file( dawn.config.input_file );
        if ( auto error = dawn.eval( source ) )
        {
            print( error.value() );
            return -3;
        }
    }
    catch ( String const& error )
    {
        print( error );
        return -2;
    }

    ArrayValue args;
    for ( int i = 2; i < argc; i++ )
        args.data.emplace_back( String{ argv[i] }, LOCATION_NONE );

    Value retval{ Int(), LOCATION_NONE };
    if ( auto error = dawn.call_func( "main", { Value{ args, LOCATION_NONE } }, &retval ) )
    {
        print( error.value() );
        return -4;
    }
    return (int) retval.to_int( dawn.engine );
}

#endif
