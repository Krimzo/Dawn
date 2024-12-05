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
#include <print>
#include <variant>
#include <ranges>


namespace dawn
{
using Bool = bool;
using Int = int64_t;
using Float = double;
using Char = wchar_t;
using String = std::wstring;

using StringRef = std::wstring_view;
using StringStream = std::wstringstream;

using IFileStream = std::wifstream;
using OFileStream = std::wofstream;

using Any = std::any;
template<typename T> using Opt = std::optional<T>;
template<typename... Args> using Variant = std::variant<Args...>;
template<typename T1, typename T2> using Pair = std::pair<T1, T2>;
template<typename T> using Ref = std::shared_ptr<T>;

template<typename T> using Array = std::vector<T>;
template<typename T> using List = std::list<T>;
template<typename K> using Set = std::unordered_set<K>;
template<typename K, typename V> using Map = std::unordered_map<K, V>;

template<typename T> using Func = std::function<T>;
}
