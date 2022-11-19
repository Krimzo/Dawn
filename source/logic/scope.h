#pragma once

#include <unordered_map>

#include "logic/variable.h"
#include "logic/function.h"


namespace faxdawn {
    class scope
    {
    public:
        std::unordered_map<std::string, variable> variables;
        std::unordered_map<std::string, function> functions;

        scope();
    };
}
