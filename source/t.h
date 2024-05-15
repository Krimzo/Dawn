#pragma once

#include <iostream>
#include <fstream>
#include <sstream>

#include <string>
#include <typeinfo>

#include <vector>
#include <unordered_set>
#include <unordered_map>

#include <optional>
#include <memory>


namespace dawn {
	using Bool = bool;

	using Int8 = int8_t;
	using Int16 = int16_t;
	using Int32 = int32_t;
	using Int64 = int64_t;
	using Int = int64_t;

	using UInt8 = uint8_t;
	using UInt16 = uint16_t;
	using UInt32 = uint32_t;
	using UInt64 = uint64_t;
	using UInt = uint64_t;

	using Float32 = float;
	using Float64 = double;
	using Float = Float32;

	using Char = wchar_t;
	using String = std::wstring;

	using StringView = std::wstring_view;
	using StringStream = std::wstringstream;

	template<typename T> using Object = std::shared_ptr<T>;
	template<typename T> using Optional = std::optional<T>;
	template<typename T1, typename T2> using Pair = std::pair<T1, T2>;

	template<typename T> using Array = std::vector<T>;
	template<typename K> using Set = std::unordered_set<K>;
	template<typename K, typename V> using Map = std::unordered_map<K, V>;
}
