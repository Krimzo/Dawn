#pragma once

#include "Faxdawn/Faxdawn.h"
#include "Lexer/Lexer.h"
#include <list>


namespace Faxdawn {
	class Parser {
	private:
		std::vector<std::string>& types;

		std::vector<Token> update_class_types(const std::vector<Token>& tokens) const;
		std::vector<Token> update_identifier_types(const std::vector<Token>& tokens) const;
		std::vector<Token> update_array_types(const std::vector<Token>& tokens) const;

	public:
		Parser(std::vector<std::string>& types);
		Parser(const Parser&) = delete;
		void operator=(const Parser&) = delete;
		~Parser();

		std::vector<Token> parse(const std::vector<Token>& tokens) const;
	};
}
