#pragma once

#include "syntax.h"

namespace dawn {
	enum class TokenType
	{
		WHITESPACE = 0,
		IDENTIFIER,
		KEYWORD,
		INTEGER,
		FLOAT,
		CHAR,
		STRING,
		OPERATOR,
		SEPARATOR,
	};
}

namespace dawn {
	struct Token
	{
		TokenType type = {};
		std::string value = {};
		size_t line_number = 0;
		size_t col_number = 0;

		Token() = default;
	};
}

namespace dawn {
	class Lexer
	{
	public:
		std::unordered_set<std::string> keywords = {};
		std::unordered_set<std::string> operators = {};
		std::unordered_set<char> separators = {};

		char identifier_extender = 0;
		char decimal_number_definer = 0;

		char literal_char = 0;
		char literal_string = 0;

		std::string line_comment = {};
		std::pair<std::string, std::string> multiline_comment = {};

		Lexer() = default;

		std::optional<std::string> process(const std::string_view& source, std::vector<Token>* tokens) const;

		void load_defualt_dawn();

	private:
		// Helper
		bool handle_line_comments(const std::string_view& source, size_t& i) const;
		bool handle_multiline_comments(const std::string_view& source, size_t& i) const;

		// Keywords
		bool iskey(const std::string& data) const;

		// Operators
		bool isop(const std::string& data) const;
		bool isop(const char value) const;

		// Separators
		bool issep(const char value) const;
	};
}

std::ostream& operator<<(std::ostream& stream, const dawn::TokenType type);
std::ostream& operator<<(std::ostream& stream, const dawn::Token& token);
