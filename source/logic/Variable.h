#pragma once

#include "dawn/Syntax.h"


namespace dawn {
    struct Variable
    {
        String type = {};
        Array<Byte> data = {};
    };
}
