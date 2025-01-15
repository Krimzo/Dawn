#include "type.h"
#include "node.h"


dawn::Bool dawn::Function::is_lambda() const
{
    return id <= 0;
}

dawn::Bool dawn::Function::is_method() const
{
    return self->type() != ValueType::NOTHING;
}
