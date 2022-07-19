#pragma once

#include "Lexer/Token.h"
#include <iostream>
#include <syncstream>
#include <string>
#include <vector>


namespace Faxdawn {
	inline const char comment = '$';
	inline const char character = '\'';
	inline const char string = '"';

	inline const std::vector<std::string> ignored = {
		"\t", "\n"
	};
	inline const std::vector<std::string> discarded = {
		"", " "
	};

	inline const std::vector<std::string> separators = {
		" ", ";", ",",
		"{", "(", "[",
		"}", ")", "]"
	};
	inline const std::vector<std::string> operators = {
		"=",
		"+", "-", "*", "/", "%",
		"+=", "-=", "*=", "/=", "%=",
		"<", ">", "<=", ">=",
		"->", "<-"
	};

	inline const std::vector<std::string> keywords = {
		"if", "elif", "else",
		"is", "not", "and", "or",
		"switch", "case",
		"while", "loop", "stop", "skip",
		"for", "in",
		"class", "pub", "maker", "self", "static",
		"none",
		"use"
	};

	template<const char END = '\n', typename... Args> inline void print(const Args&... args) {
		(std::osyncstream(std::cout) << ... << args) << END;
	}
	template<typename... Args> inline void warning(bool state, const Args&... args) {
		if (state) {
			print(args...);
		}
	}
	template<const int E = 1, typename... Args> inline void error(bool state, const Args&... args) {
		if (state) {
			print(args...);
			std::cin.get();
			exit(E);
		}
	}
	template<const int E = 1> inline void error(bool state, const Faxdawn::Token& token) {
		error(state, "Error ", token.getType(), " \"", token.value, "\" at line ", token.line);
	}
}
