#include "dawn.h"

#define DEBUG_TESTS 0

using namespace dawn; // Only in this case since it is not a header file.

static constexpr StringRef DEFAULT_INPUT = ".";
static constexpr StringRef DIR_CONFIG_FILENAME = "dawn.ini";

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

    ArrayValue args;
    for ( int i = 0; i < argc; i++ )
        args.data.emplace_back( String{ argv[i] }, LOCATION_NONE );

    Value retval{ Int(), LOCATION_NONE };
    if ( auto error = dawn.call_func( "main", { Value{ args, LOCATION_NONE } }, &retval ) )
    {
        print( error.value() );
        return -2;
    }
    return (int) retval.to_int( dawn.engine );
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

    if ( dawn.config.input_file.empty() )
        dawn.config.input_file = DEFAULT_INPUT;

    if ( fs::is_directory( dawn.config.input_file ) )
    {
        if ( auto error = dawn.config.from_file( format( dawn.config.input_file, "/", DIR_CONFIG_FILENAME ) ) )
        {
            print( error.value() );
            return -2;
        }
    }

    try {
        const Source source = Source::from_file( dawn.config.input_file );
        if ( auto error = dawn.eval( source ) )
        {
            print( error.value() );
            return -4;
        }
    }
    catch ( String const& error )
    {
        print( error );
        return -3;
    }

    ArrayValue args;
    for ( int i = 0; i < argc; i++ )
        args.data.emplace_back( String{ argv[i] }, LOCATION_NONE );

    Value retval{ Int(), LOCATION_NONE };
    if ( auto error = dawn.call_func( "main", { Value{ args, LOCATION_NONE } }, &retval ) )
    {
        print( error.value() );
        return -5;
    }
    return (int) retval.to_int( dawn.engine );
}

#endif
