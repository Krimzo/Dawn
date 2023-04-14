#pragma once

#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>
#include <unordered_set>
#include <unordered_map>

#include <functional>
#include <thread>
#include <regex>


namespace dawn {
    using Bool = bool;
    using Byte = uint8_t;

    using Int = int64_t;
    using Float = double;

    using Char = wchar_t;
    using String = std::wstring;

    using StringStream = std::wstringstream;

    template<typename T> using Lambda = std::function<T>;
    template<typename T> using Array = std::vector<T>;
    template<typename K> using Set = std::unordered_set<K>;
    template<typename K, typename V> using Map = std::unordered_map<K, V>;
}
