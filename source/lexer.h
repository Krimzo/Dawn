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
}

namespace dawn {
	struct Token
	{
		TokenType type;
		String value;
		Int line_number = 0;
	};
}

namespace dawn {
	struct LanguageDef
	{
		Set<String> keywords;
		Set<String> operators;
		Set<String> separators;
		String identifier_extender;
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

		Opt<String> tokenize(const StringView& source, Array<Token>& tokens) const;

	private:
		Bool handle_line_comments(const StringView& source, Int& i) const;
		Bool handle_multiline_comments(const StringView& source, Int& i, Int& line_counter) const;

		Bool iskey(const StringView& data) const;
		Bool isop(const StringView& data) const;
		Bool issep(const StringView& value) const;
	};
}

namespace dawn {
	bool operator==(dawn::Char lhs, const dawn::StringView& rhs);
	std::wostream& operator<<(std::wostream& stream, const dawn::TokenType type);
	std::wostream& operator<<(std::wostream& stream, const dawn::Token& token);
}
