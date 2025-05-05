#include "_main.h"


int main( int argc, char** argv )
{
#ifdef DAWN_SHIP
    return dawn::_shp_main( argc, argv );
#else
    return dawn::_dev_main( argc, argv );
#endif
}
