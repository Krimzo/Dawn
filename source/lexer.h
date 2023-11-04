#pragma once

#include <iostream>
#include <vector>
#include <unordered_set>

enum class TokenType
{
	WHITESPACE = 0,
	IDENTIFIER,
	KEYWORD,
	NUMBER,
	CHAR,
	STRING,
	OPERATOR,
	SEPARATOR,
};

struct Token
{
	TokenType type = {};
	std::string value = {};

	Token() = default;
};

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

	std::vector<Token> process(const std::string_view& source) const;
	void process(const std::string_view& source, std::vector<Token>& tokens) const;

	void load_defualt_dawn();

private:
	// Helper
	bool handle_line_comments(size_t& i, const std::string_view& source, std::vector<Token>& tokens) const;
	bool handle_multiline_comments(size_t& i, const std::string_view& source, std::vector<Token>& tokens) const;

	// Keywords
	bool iskey(const std::string& data) const;

	// Operators
	bool isop(const std::string& data) const;
	bool isop(const char value) const;

	// Separators
	bool issep(const char value) const;
};

std::ostream& operator<<(std::ostream& stream, const TokenType type);
std::ostream& operator<<(std::ostream& stream, const Token& token);
