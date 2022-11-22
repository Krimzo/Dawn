﻿#pragma once

#include "logic/instruction.h"

#include <vector>


namespace faxdawn {
    struct function
    {
        std::vector<instruction> instructions;

        function() = default;

        void execute() const;
    };
}
