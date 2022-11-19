#pragma once

#include <string>
#include <vector>


namespace faxdawn {
    struct variable
    {
        std::string type;
        std::vector<uint8_t> data;

        variable();
        variable(const std::string& type, const void* data, size_t size);
    };
}
