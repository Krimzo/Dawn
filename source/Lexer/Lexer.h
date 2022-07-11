#pragma once

#include "Lexer/Token.h"

#include <vector>


namespace Faxdawn {
	inline const char comment = '$';
	inline const char character = '\'';
	inline const char string = '"';

	/* ----- */
	inline const std::vector<std::string> ignored = {
		"\t", "\n"
	};
	inline const std::vector<std::string> discarded = {
		"", " "
	};
	bool IsIgnored(char value);
	bool IsIgnored(const std::string& value);
	bool IsDiscarded(const std::string& value);

	/* ----- */
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
	bool IsSeparator(char value);
	bool IsOperator(char value);
	bool IsSeparator(const std::string& value);
	bool IsOperator(const std::string& value);

	/* ----- */
	inline const std::vector<std::string> keywords = {
		"if", "elif", "else",
		"is", "not", "and", "or",
		"switch", "case",
		"while", "loop", "stop", "skip",
		"for", "in",
		"class", "pub", "self", "static",
		"use"
	};
	inline std::vector<std::string> types = {
		"bool", "byte", "char",
		"int", "float",
		"string", "lambda"
	};
	bool IsKeyword(const std::string& value);
	bool IsType(const std::string& value);
	bool IsLiteral(const std::string& value);

	/* ----- */
	class Lexer {
	private:
		std::vector<std::string> split(const std::string& source) const;

	public:
		Lexer();
		Lexer(const Lexer&) = delete;
		void operator=(const Lexer&) = delete;
		~Lexer();

		std::vector<Token> generate(const std::string& source) const;
	};
}
