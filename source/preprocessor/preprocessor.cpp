#include "preprocessor/preprocessor.h"


std::string faxdawn::preprocessor::process(const std::string& source) const
{
    std::string result = source;
    for (auto& macro : macros) {
        std::replace(result.begin(), result.end(), macro.first, macro.second);
    }
    return result;
}
