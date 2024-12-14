#include "_main.h"


int dawn::_dev_main( int argc, char** argv )
{
    auto start_time = std::chrono::high_resolution_clock::now();

    Dawn dawn;

    if ( auto error = dawn.eval_file( "examples/min_example.dw" ) )
    {
        print( error.value() );
        return -1;
    }

    if ( auto error = dawn.call_func( "main" ) )
    {
        print( error.value() );
        return -2;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    print( duration );

    return 0;
}
