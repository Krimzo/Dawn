#pragma once

#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>
#include <list>
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

    using InputStream = std::wistream;
    using OutputStream = std::wostream;

    using FileOutputStream = std::wofstream;
    using FileInputStream = std::wifstream;

    using StringStream = std::wstringstream;

    template<typename T>
    using Lambda = std::function<T>;

    template<typename T>
    using Array = std::vector<T>;

    template<typename T>
    using List = std::list<T>;

    template<typename K>
    using Set = std::unordered_set<K>;

    template<typename K, typename V>
    using Map = std::unordered_map<K, V>;
}
