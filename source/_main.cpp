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

    ValueBox retval;
    if ( auto error = dawn.call_func( L"main", {}, retval ) )
    {
        print( error.value() );
        return 2;
    }
}
