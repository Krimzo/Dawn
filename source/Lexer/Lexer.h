#pragma once

#include "Faxdawn/Faxdawn.h"
#include "Lexer/Token.h"
#include <sstream>


namespace Faxdawn {
	bool IsIgnored(char value);
	bool IsIgnored(const std::string& value);
	bool IsDiscarded(const std::string& value);

	bool IsSeparator(char value);
	bool IsOperator(char value);
	bool IsSeparator(const std::string& value);
	bool IsOperator(const std::string& value);

	bool IsKeyword(const std::string& value);
	bool IsType(const std::vector<std::string>& types, const std::string& value);
	bool IsLiteral(const std::string& value);

	class Lexer {
	private:
		std::vector<std::string>& types;

		std::vector<Token> split(const std::string& source) const;

	public:
		Lexer(std::vector<std::string>& types);
		Lexer(const Lexer&) = delete;
		void operator=(const Lexer&) = delete;
		~Lexer();

		std::vector<Token> generate(const std::string& source) const;
	};
}
