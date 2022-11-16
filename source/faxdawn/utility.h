#pragma once

#include "lexer/token.h"

#include <iostream>

#undef assert

namespace faxdawn::utility {
	template<bool NewLine = true, typename... Args> void print(const Args&... args)
	{
		(std::cout << ... << args);
		if constexpr (NewLine) {
            std::cout << std::endl;
		}
	}

	template<typename... Args> void warning(const bool state, const Args&... args)
	{
		if (state) {
			print(args...);
		}
	}

	template<typename... Args> void assert(const bool state, const Args&... args)
	{
		if (state) {
			print(args...);
			std::cin.get();
			exit(1);
		}
	}

	inline void assert(const bool state, const token& token) {
		assert(state, "error ", token.type_description(), " \"", token.value, "\" at line ", token.line);
	}
}
