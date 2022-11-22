#pragma once

#include "logic/instruction.h"

#include <vector>


namespace faxdawn {
    struct function
    {
        std::vector<instruction> instructions;

        function();

        void execute() const;
    };
}
