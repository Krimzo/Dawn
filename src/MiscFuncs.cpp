#include "MiscFuncs.h"

#include <iostream>
#include <string>


bool StringReplace(std::string& str, const std::string& from, const std::string& to) {
    // Input checking
    if (from.empty()) {
        return false;
    }

    // Replacing
    size_t position = 0;
    int replaceCounter = 0;
    while ((position = str.find(from, position)) != std::string::npos) {
        str.replace(position, from.length(), to);
        position += to.length();
        replaceCounter++;
    }

    // Returning the replace status
    return replaceCounter;
}
