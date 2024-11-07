#pragma once

#include "syntax.h"


namespace dawn {
	enum struct TokenType
	{
		INT = 0,
		FLOAT,
		CHAR,
		STRING,
		IDENTIFIER,
		SEPARATOR,
		OPERATOR,
		KEYWORD,
	};

	std::wostream& operator<<(std::wostream& stream, const TokenType type);
}

namespace dawn {
	struct Token
	{
		TokenType type;
		String value;
		Int line_number = 0;
	};

	std::wostream& operator<<(std::wostream& stream, const Token& token);
}

namespace dawn {
	struct LanguageDef
	{
		Set<String> keywords;
		Set<String> operators;
		Set<String> separators;
		String identifier_separator;
		String decimal_separator;
		String literal_char;
		String literal_string;
		String line_comment;
		Pair<String, String> multiline_comment;

		static LanguageDef default_def();
	};
}

namespace dawn {
	struct Lexer
	{
		LanguageDef lang_def = LanguageDef::default_def();

		Opt<String> tokenize(const StringRef& source, Array<Token>& tokens);

	private:
		Bool is_space(const StringRef& source, Int i);
		void extract_space(const StringRef& source, Array<Token>& tokens, Int& lines, Int& i);

		Bool is_comment(const StringRef& source, Int i);
		void extract_comment(const StringRef& source, Array<Token>& tokens, Int& lines, Int& i);

		Bool is_mlcomment(const StringRef& source, Int i);
		void extract_mlcomment(const StringRef& source, Array<Token>& tokens, Int& lines, Int& i);

		Bool is_word(const StringRef& source, Int i);
		void extract_word(const StringRef& source, Array<Token>& tokens, Int& lines, Int& i);

		Bool is_number(const StringRef& source, Int i);
		void extract_number(const StringRef& source, Array<Token>& tokens, Int& lines, Int& i);

		Bool is_char(const StringRef& source, Int i);
		void extract_char(const StringRef& source, Array<Token>& tokens, Int& lines, Int& i);

		Bool is_string(const StringRef& source, Int i);
		void extract_string(const StringRef& source, Array<Token>& tokens, Int& lines, Int& i);

		Bool is_operator(const StringRef& data, Int i);
		void extract_operator(const StringRef& data, Array<Token>& tokens, Int& lines, Int& i);

		Bool is_separator(const StringRef& value, Int i);
		void extract_separator(const StringRef& value, Array<Token>& tokens, Int& lines, Int& i);
	};
}

namespace dawn {
	bool operator==(Char lhs, const StringRef& rhs);
}
