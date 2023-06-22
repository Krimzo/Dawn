#pragma once

#include "dawn/Syntax.h"


namespace dawn {
    class Preprocessor : public Map<String, String>
    {
    public:
        String Process(const String& source) const;
    };
}
