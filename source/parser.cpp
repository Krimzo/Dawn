#include "parser.h"


std::shared_ptr<dawn::ASTNode> dawn::Parser::parse_expression(const std::vector<Token>& tokens)
{
	if (tokens.empty()) {
		return nullptr;
	}

	auto& token = tokens.front();
	switch (token.type)
	{
	case TokenType::KEYWORD:    return parse_expression_keyword(tokens);
	case TokenType::IDENTIFIER: return parse_expression_identifier(tokens);
	}

	return log_error("Expression can't start with a ", token);
}

std::shared_ptr<dawn::ASTNode> dawn::Parser::parse_expression_keyword(const std::vector<Token>& tokens)
{

	return nullptr;
}

std::shared_ptr<dawn::ASTNode> dawn::Parser::parse_expression_identifier(const std::vector<Token>& tokens)
{

	return nullptr;
}
