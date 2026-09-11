#include "syntax.h"

static dawn::Int _prec_counter = 0;

dawn::Int dawn::_get_prec()
{
    return _prec_counter;
}

dawn::Int dawn::_incr_get_prec()
{
    return ++_prec_counter;
}
