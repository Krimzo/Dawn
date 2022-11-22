#include "preprocessor/preprocessor.h"

#include <regex>


std::string faxdawn::preprocessor::process(const std::string& source) const
{
    std::string result = source;
    for (auto& macro : macros) {
        std::regex_replace(result, std::regex(macro.first), macro.second);
    }
    return result;
}
