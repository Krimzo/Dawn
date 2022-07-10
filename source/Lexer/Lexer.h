#pragma once

#include "Lexer/Token.h"

#include <vector>


namespace Faxdawn {
	inline const char comment = '$';
	inline const char ignored[] = {
		'\t', '\n'
	};

	inline const char separators[] = {
		' ', ';', ',',
		'{', '(', '[',
		'}', ')', ']'
	};
	inline const std::string operators[] = {
		"=",
		"+", "-", "*", "/", "%",
		"+=", "-=", "*=", "/=", "%=",
		"==", "!=", "<", ">", "<=", ">=",
		"->"
	};
	inline const std::string keywords[] = {
		"if", "elif", "else",
		"not", "and", "or",
		"switch", "case",
		"for", "while", "do", "loop", "break", "continue",
		"def", "return",
		"class", "pub", "self",
		"use"
	};
	inline std::vector<std::string> types = {
		"bool", "byte", "char",
		"int", "float",
		"string", "lambda"
	};

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
