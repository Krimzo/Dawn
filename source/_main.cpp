#include "dawn.h"


int main( int argc, char** argv )
{
    using namespace dawn;

    Dawn dawn;
    if ( auto error = dawn.eval_file( L"examples/min_example.dw" ) )
    {
        print( error.value() );
        return 1;
    }

    auto start_time = std::chrono::high_resolution_clock::now();

    ValueRef retval;
    if ( auto error = dawn.call_func( L"main", {}, retval ) )
    {
        print( error.value() );
        return 2;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    print( duration );
}
