#include "Lexer/Token.h"


Faxdawn::Token::Token() {

}
Faxdawn::Token::~Token() {

}

std::string Faxdawn::Token::getType() const {
	switch (type) {
	case Token::Type::Separator:
		return "Separator";
	case Token::Type::Operator:
		return "Operator";
	case Token::Type::Keyword:
		return "Keyword";
	case Token::Type::Type:
		return "Type";
	case Token::Type::Array:
		return "Array";
	case Token::Type::Literal:
		return "Literal";
	case Token::Type::Identifier:
		return "Identifier";
	}
	return "Unknown";
}

std::ostream& Faxdawn::operator<<(std::ostream& stream, const Token& token) {
	stream << "[ " << token.getType() << " : \"" << token.value << "\" ]";
	return stream;
}
