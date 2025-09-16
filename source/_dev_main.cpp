#include "_main.h"


int dawn::_dev_main( int argc, char** argv )
{
    auto start_time = ch::high_resolution_clock::now();

    Dawn dawn;

    if ( auto error = dawn.eval_file(
#if _DEBUG
#if 0
        "examples/tests.dw"
#else
        "examples/dev.dw"
#endif
#else
        "examples/bench.dw"
#endif
    ) )
    {
        print( error.value() );
        return -1;
    }

    if ( auto error = dawn.call_func( "main" ) )
    {
        print( error.value() );
        return -2;
    }

    auto end_time = ch::high_resolution_clock::now();
    auto duration = ch::duration_cast<ch::microseconds>( end_time - start_time );
    print( "Exec dur: ", duration );

    return 0;
}
