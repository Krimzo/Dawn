#pragma once

#include "lexer/Token.h"


namespace dawn {
	template<bool NewLine = true, typename... Args> void Print(const Args&... args)
	{
		(std::wcout << ... << args);
		if constexpr (NewLine) {
            std::wcout << std::endl;
		}
	}

	template<typename... Args> void Warning(const bool state, const Args&... args)
	{
		if (state) {
			Print(args...);
		}
	}

	template<typename... Args> void Assert(const bool state, const Args&... args)
	{
		if (state) {
			Print(args...);
			std::cin.get();
			exit(1);
		}
	}

	inline void Assert(const bool state, const Token& token)
	{
		Assert(state, "Error ", GetTokenDescription(token.type), " \"", token.value, "\" at line ", token.line);
	}
}

namespace dawn {
	template<typename T>
	std::wostream& operator<<(std::wostream& stream, const Array<T>& buffer)
	{
		if (buffer.empty()) {
			stream << L"[]";
			return stream;
		}

		stream << L"[";
		for (size_t i = 0; i < buffer.size() - 1; i++) {
			stream << buffer[i] << L", ";
		}
		stream << buffer.back() << L"]";
		return stream;
	}
}
