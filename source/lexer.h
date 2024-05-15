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
		String value = {};
		UInt line_number = 0;

		Token() = default;
	};
}

namespace dawn {
	class Lexer
	{
	public:
		Set<String> keywords = {};
		Set<String> operators = {};
		Set<Char> separators = {};

		Char identifier_extender = 0;
		Char decimal_number_definer = 0;

		Char literal_char = 0;
		Char literal_string = 0;

		String line_comment = {};
		Pair<String, String> multiline_comment = {};

		Lexer() = default;

		Optional<String> process(const StringView& source, Array<Token>* tokens) const;

		void load_defualt_dawn();

	private:
		// Helper
		Bool handle_line_comments(const StringView& source, UInt& i) const;
		Bool handle_multiline_comments(const StringView& source, UInt& i, UInt& new_line_counter) const;

		// Keywords
		Bool iskey(const String& data) const;

		// Operators
		Bool isop(const String& data) const;
		Bool isop(const Char value) const;

		// Separators
		Bool issep(const Char value) const;
	};
}

std::wostream& operator<<(std::wostream& stream, const dawn::TokenType type);
std::wostream& operator<<(std::wostream& stream, const dawn::Token& token);
