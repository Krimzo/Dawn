#include "btype.h"


dawn::Bool dawn::BFunction::is_lambda() const
{
    return id <= 0;
}

dawn::Bool dawn::BFunction::is_method() const
{
    return !self_val.empty();
}

dawn::Bool dawn::BFunction::is_unary_op() const
{
    return args.size() == 1;
}
