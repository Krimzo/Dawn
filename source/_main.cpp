#include "_main.h"


int main( int argc, char** argv )
{
    if constexpr ( true )
        return dawn::_dev_main( argc, argv );
    else
        return dawn::_shp_main( argc, argv );
}
