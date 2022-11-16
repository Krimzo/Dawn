#pragma once

#include "lexer/token.h"

#include <vector>
#include <string>


namespace faxdawn::syntax {
	inline constexpr char comment = '$';
	inline constexpr char character = '\'';
	inline constexpr char string = '"';

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
		"=", ":",
		"?", "!",
		"+", "-", "*", "^", "/", "%",
		"+=", "-=", "*=", "/=", "%=",
		"<", ">", "<=", ">="
	};

	inline const std::vector<std::string> keywords = {
		"if", "elif", "else",
		"is", "not", "and", "or",
		"compare", "check", "other",
		"while", "loop", "stop", "skip",
		"type", "pub", "maker",
		"self", "static",
		"none",
		"use"
	};

	inline const std::vector<std::string> types = {
		"bool", "byte",
		"int", "float",
		"char", "String",
		"Function",
	};
}

namespace faxdawn::syntax {
	token_type get_token_type(const std::vector<std::string>& all_types, const std::string& value);
	
	bool is_discarded(const std::string& value);
	
	bool is_ignored(char value);
	bool is_ignored(const std::string& value);

	bool is_separator(char value);
	bool is_separator(const std::string& value);

	bool is_operator(char value);
	bool is_operator(const std::string& value);

	bool is_keyword(const std::string& value);
	
	bool is_type(const std::vector<std::string>& all_types, const std::string& value);
	
	bool is_literal(const std::string& value);
}
