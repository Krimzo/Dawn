#pragma once

#include <string>
#include <unordered_map>


namespace faxdawn {
    class preprocessor {
    public:
        std::unordered_map<std::string, std::string> macros = {};

        preprocessor() = default;

        std::string process(const std::string& source) const;
    };
}
