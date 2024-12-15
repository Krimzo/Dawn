#pragma once

#include <any>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <typeinfo>
#include <functional>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <optional>
#include <memory>
#include <variant>
#include <ranges>
#include <random>
#include <chrono>
#include <filesystem>


namespace dawn
{
namespace ch = std::chrono;
namespace fs = std::filesystem;

using Bool = bool;
using Int = int64_t;
using Float = double;
using Char = char;
using String = std::string;

using StringRef = std::string_view;
using StringStream = std::stringstream;

using IFileStream = std::ifstream;
using OFileStream = std::ofstream;

using Any = std::any;
template<typename T> using Opt = std::optional<T>;
template<typename... Args> using Variant = std::variant<Args...>;
template<typename T1, typename T2> using Pair = std::pair<T1, T2>;

template<typename T> using Array = std::vector<T>;
template<typename K> using Set = std::unordered_set<K>;
template<typename K, typename V> using Map = std::unordered_map<K, V>;

template<typename T> using Func = std::function<T>;
}
