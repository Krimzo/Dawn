#include "preprocessor/Preprocessor.h"


dawn::String dawn::Preprocessor::Process(const String& source) const {
    String result = source;
    for (const auto& [key, value] : *this) {
        result = std::regex_replace(result, std::wregex(key), value);
    }
    return result;
}
