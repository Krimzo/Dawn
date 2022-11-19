#include "logic/variable.h"


faxdawn::variable::variable()
{}

faxdawn::variable::variable(const std::string& type, const void* data, const size_t size)
    : type(type)
{
    this->data.resize(size);
    memcpy(this->data.data(), data, size);
}
