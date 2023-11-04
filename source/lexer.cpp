#include "lexer.h"


std::vector<Token> Lexer::process(const std::string_view& source) const
{
	std::vector<Token> tokens = {};
	this->process(source, tokens);
	return tokens;
}

void Lexer::process(const std::string_view& source, std::vector<Token>& tokens) const
{
	for (size_t i = 0; i < source.size(); i++) {
		// Range check
		if (source[i] < 0 || source[i] > 127) {
			continue;
		}

		// Whitespace
		if (isspace(source[i])) {
			Token token = {};
			for (; i < source.size(); i++) {
				if (isspace(source[i])) {
					token.value.push_back(source[i]);
				}
				else {
					break;
				}
			}
			token.type = TokenType::WHITESPACE;
			tokens.push_back(token);
		}

		// Single line comments
		if (handle_line_comments(i, source, tokens)) {
			i -= 1;
			continue;
		}

		// Multiline comments
		if (handle_multiline_comments(i, source, tokens)) {
			i -= 1;
			continue;
		}

		// Identifiers and Keywords
		if (isalpha(source[i])) {
			Token token = {};
			for (; i < source.size(); i++) {
				if (isalnum(source[i]) || source[i] == identifier_extender) {
					token.value.push_back(source[i]);
				}
				else {
					break;
				}
			}
			token.type = iskey(token.value) ? TokenType::KEYWORD : TokenType::IDENTIFIER;
			tokens.push_back(token);
		}

		// Numbers
		if (isdigit(source[i])) {
			Token token = {};
			for (; i < source.size(); i++) {
				if (isdigit(source[i]) || source[i] == decimal_number_definer) {
					token.value.push_back(source[i]);
				}
				else {
					break;
				}
			}
			token.type = TokenType::NUMBER;
			tokens.push_back(token);
		}

		// Chars
		if (source[i] == literal_char) {
			Token token = {};
			token.value.push_back(source[i++]);
			for (; i < source.size(); i++) {
				token.value.push_back(source[i]);
				if (source[i] == literal_char) {
					i += 1;
					break;
				}
			}
			token.type = TokenType::CHAR;
			tokens.push_back(token);
		}

		// Strings
		if (source[i] == literal_string) {
			Token token = {};
			token.value.push_back(source[i++]);
			for (; i < source.size(); i++) {
				token.value.push_back(source[i]);
				if (source[i] == literal_string) {
					i += 1;
					break;
				}
			}
			token.type = TokenType::STRING;
			tokens.push_back(token);
		}

		// Operators
		if (isop(source[i])) {
			Token token = {};
			for (; i < source.size(); i++) {
				token.value.push_back(source[i]);
				if (!isop(token.value)) {
					token.value.pop_back();
					i -= 1;
					break;
				}
			}
			token.type = TokenType::OPERATOR;
			tokens.push_back(token);
		}

		// Separators
		if (issep(source[i])) {
			Token token = {};
			token.value = std::string(1, source[i]);
			token.type = TokenType::SEPARATOR;
			tokens.push_back(token);
		}
	}
}

bool Lexer::handle_line_comments(size_t& i, const std::string_view& source, std::vector<Token>& tokens) const
{
	if (line_comment.contains(source[i])) {
		std::string temp_holder = {};
		for (; i < source.size(); i++) {
			temp_holder.push_back(source[i]);
			if (temp_holder == line_comment) {
				for (; i < source.size(); i++) {
					if (source[i] == '\n') {
						i += 1;
						return true;
					}
				}
			}
			if (!line_comment.contains(temp_holder)) {
				i -= temp_holder.size() - 1;
				return false;
			}
		}
	}
	return false;
}

bool Lexer::handle_multiline_comments(size_t& i, const std::string_view& source, std::vector<Token>& tokens) const
{
	if (multiline_comment.first.contains(source[i])) {
		std::string left_holder = {};
		for (; i < source.size(); i++) {
			left_holder.push_back(source[i]);
			if (left_holder == multiline_comment.first) {
				std::string right_holder = {};
				for (; i < source.size(); i++) {
					const bool had_contained = !right_holder.empty() && multiline_comment.second.contains(right_holder);
					right_holder.push_back(source[i]);
					if (right_holder == multiline_comment.second) {
						i += 1;
						return true;
					}
					if (!multiline_comment.second.contains(right_holder)) {
						right_holder.clear();
						if (had_contained) {
							i -= 1;
						}
					}
				}
			}
			if (!multiline_comment.first.contains(left_holder)) {
				i -= left_holder.size() - 1;
				return false;
			}
		}
	}
	return false;
}

// Keywords
bool Lexer::iskey(const std::string& data) const
{
	return keywords.contains(data);
}

// Operators
bool Lexer::isop(const std::string& data) const
{
	return operators.contains(data);
}

bool Lexer::isop(const char value) const
{
	return this->isop(std::string(1, value));
}

// Separators
bool Lexer::issep(const char value) const
{
	return separators.contains(value);
}

void Lexer::load_defualt_dawn()
{
	keywords = {
		// Modules
		"module",
		"import",
		"internal",

		// Functions
		"def",
		"return",

		// Variables
		"let",
		"var",
		"null",
		"new",

		// If
		"if",
		"else",
		"elif",

		// Switch
		"switch",
		"case",
		"default",

		// Loops
		"for",
		"while",
		"loop",
		"continue",
		"break",

		// POD objects
		"enum",
		"struct",

		// Class objects
		"interface",
		"implement",
		"class",
		"init",
		"deinit",
		"self",

		// Integers
		"int8",
		"int16",
		"int32",
		"int64",

		// Unsigned integers
		"uint8",
		"uint16",
		"uint32",
		"uint64",

		// Floating point
		"float",
		"double",

		// Text
		"char",
		"string",

		// Bools
		"true",
		"false",
	};

	operators = {
		// Bits
		"~",
		"|",
		"^",
		"&",
		"<<",
		">>",
		"~=",
		"|=",
		"^=",
		"&=",
		"<<=",
		">>=",

		// Math
		"+",
		"-",
		"*",
		"/",
		"%",
		"+=",
		"-=",
		"*=",
		"/=",
		"%=",

		// Bools
		"!",
		"<",
		">",
		"!=",
		"<=",
		">=",
		"==",
		"&&",
		"||",

		// Access
		"->",
	};

	separators = {
		// Access
		'=',
		',',
		';',
		'.',
		':',

		// Control
		'(',
		')',
		'{',
		'}',
		'[',
		']',
	};

	identifier_extender = '_';
	decimal_number_definer = '.';

	literal_char = '\'';
	literal_string = '"';

	line_comment = "//";
	multiline_comment = { "/*", "*/" };
}

std::ostream& operator<<(std::ostream& stream, const TokenType type)
{
	switch (type) {
	case TokenType::WHITESPACE:
		stream << "Whitespace";
		break;
	case TokenType::IDENTIFIER:
		stream << "Identifier";
		break;
	case TokenType::KEYWORD:
		stream << "Keyword";
		break;
	case TokenType::NUMBER:
		stream << "Number";
		break;
	case TokenType::CHAR:
		stream << "Char";
		break;
	case TokenType::STRING:
		stream << "String";
		break;
	case TokenType::OPERATOR:
		stream << "Operator";
		break;
	case TokenType::SEPARATOR:
		stream << "Separator";
		break;
	}
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const Token& token)
{
	stream << '[';
	if (token.type == TokenType::WHITESPACE) {
		for (size_t i = 0; i < token.value.size() - 1; i++) {
			stream << "'\\" << static_cast<int>(token.value[i]) << "' ";
		}
		if (!token.value.empty()) {
			stream << "'\\" << static_cast<int>(*--token.value.end()) << "'";
		}
	}
	else {
		stream << token.value;
	}
	stream << ", " << token.type << ']';
	return stream;
}
