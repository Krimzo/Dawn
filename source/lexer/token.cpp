#include "lexer/token.h"


std::string faxdawn::token::type_description() const
{
	switch (type) {
	case token_type::Separator:
		return "Separator";
	case token_type::Operator:
		return "Operator";
	case token_type::Keyword:
		return "Keyword";
	case token_type::Type:
		return "Type";
	case token_type::Array:
		return "Array";
	case token_type::Literal:
		return "Literal";
	case token_type::Identifier:
		return "Identifier";
	}
	return "Unknown";
}

std::ostream& faxdawn::operator<<(std::ostream& stream, const token& token)
{
	stream << "(" << token.type_description() << " : " << token.value << ")";
	return stream;
}
