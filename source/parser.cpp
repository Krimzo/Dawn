#include "parser.h"


const std::vector<std::string>& dawn::Parser::get_errors() const
{
	return m_errors;
}

void dawn::Parser::print_errors() const
{
	for (const auto& error : m_errors) {
		std::cout << error << std::endl;
	}
}

void dawn::Parser::assign_tokens(const std::vector<Token>& tokens)
{
	m_tokens.clear();
	m_errors.clear();

	for (const auto& token : tokens) {
		if (token.type != TokenType::WHITESPACE) {
			m_tokens.push_back(token);
		}
	}
}

std::shared_ptr<dawn::ASTNode> dawn::Parser::parse()
{
	if (m_tokens.empty()) {
		return nullptr;
	}

	
	return nullptr;
}
