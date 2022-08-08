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

	template<bool NewLine = true, typename... Args> inline void Print(const Args&... args) {
		std::osyncstream stream(std::cout);
		(stream << ... << args);
		if constexpr (NewLine) {
			stream << std::endl;
		}
	}

	template<typename... Args> inline void Warning(bool state, const Args&... args) {
		if (state) {
			Print(args...);
		}
	}

	template<typename... Args> inline void Error(bool state, const Args&... args) {
		if (state) {
			Print(args...);
			std::cin.get();
			exit(1);
		}
	}

	inline void Error(bool state, const Faxdawn::Token& token) {
		Error(state, "Error ", token.getType(), " \"", token.value, "\" at line ", token.line);
	}
}
