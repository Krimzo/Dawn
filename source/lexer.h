#pragma once

#include "syntax.h"


namespace dawn {
	struct LexError
	{
		String msg;

		template<typename... Args>
		LexError(Int line_number, Char c, const Args&... args)
		{
			StringStream stream;
			stream << "Lex error at line [" << line_number
				<< "] and char [" << from_escaping(c) << "]: ";
			(stream << ... << args);
			msg = stream.str();
		}

		inline operator const String& () const
		{
			return msg;
		}
	};

	std::wostream& operator<<(std::wostream& stream, const LexError& error);
}

namespace dawn {
	enum struct TokenType
	{
		INTEGER = 0,
		FLOAT,
		CHAR,
		STRING,
		KEYWORD,
		TYPE,
		FUNCTION,
		NAME,
		OPERATOR,
	};

	constexpr Color to_color(TokenType type)
	{
		switch (type)
		{
		case TokenType::INTEGER:  return {240, 128, 128};
		case TokenType::FLOAT:    return {255, 182, 193};
		case TokenType::CHAR:     return {221, 160, 221};
		case TokenType::STRING:   return {173, 216, 230};
		case TokenType::KEYWORD:  return {144, 238, 144};
		case TokenType::TYPE:     return {255, 222, 173};
		case TokenType::FUNCTION: return {175, 238, 238};
		case TokenType::NAME:     return {238, 221, 130};
		case TokenType::OPERATOR: return {211, 211, 211};
		}
		return { 204, 204, 204 };
	}

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
		Set<String> types;
		Set<String> operators;
		String separator_identifier;
		String separator_number;
		String literal_char;
		String literal_string;
		String comment_line;
		Pair<String, String> comment_multiline;

		static LanguageDef dawn();
	};
}

namespace dawn {
	struct Lexer
	{
		LanguageDef lang_def = LanguageDef::dawn();

		Opt<LexError> tokenize(const StringRef& source, Array<Token>& tokens);

	private:
		Bool is_space(const StringRef& source, Int i);
		Opt<LexError> extract_space(const StringRef& source, Array<Token>& tokens, Int& line, Int& i);

		Bool is_comment(const StringRef& source, Int i);
		Opt<LexError> extract_comment(const StringRef& source, Array<Token>& tokens, Int& line, Int& i);

		Bool is_mlcomment(const StringRef& source, Int i);
		Opt<LexError> extract_mlcomment(const StringRef& source, Array<Token>& tokens, Int& line, Int& i);

		Bool is_word(const StringRef& source, Int i);
		Opt<LexError> extract_word(const StringRef& source, Array<Token>& tokens, Int& line, Int& i);

		Bool is_number(const StringRef& source, Int i);
		Opt<LexError> extract_number(const StringRef& source, Array<Token>& tokens, Int& line, Int& i);

		Bool is_char(const StringRef& source, Int i);
		Opt<LexError> extract_char(const StringRef& source, Array<Token>& tokens, Int& line, Int& i);

		Bool is_string(const StringRef& source, Int i);
		Opt<LexError> extract_string(const StringRef& source, Array<Token>& tokens, Int& line, Int& i);

		Bool is_operator(const StringRef& source, Int i);
		Opt<LexError> extract_operator(const StringRef& source, Array<Token>& tokens, Int& line, Int& i);
	};
}
