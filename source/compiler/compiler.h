#pragma once

#include "logic/instruction.h"
#include "lexer/token.h"

#include <vector>


namespace faxdawn {
    class compiler
    {
    public:
        compiler();

        std::vector<instruction> compile(const std::vector<token>& tokens) const;
    };
}
