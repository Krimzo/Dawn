#include "helper.h"
#include <windows.h>


dawn::Color dawn::get_token_color(const Token& token)
{
	switch (token.type)
	{
	case TokenType::IDENTIFIER: return Color(235, 235, 235);
	case TokenType::KEYWORD:    return Color( 85, 155, 215);
	case TokenType::INTEGER:    return Color(180, 205, 170);
	case TokenType::FLOAT:      return Color(180, 205, 170);
	case TokenType::CHAR:       return Color(230, 200, 190);
	case TokenType::STRING:     return Color(230, 200, 190);
	case TokenType::OPERATOR:   return Color(245, 245, 245);
	case TokenType::SEPARATOR:  return Color(245, 245, 245);
	}
	return {};
}

void dawn::display_colored(const Array<Token>& tokens)
{
	for (auto& token : tokens) {
		std::wcout << get_token_color(token);
		if (token.type == TokenType::WHITESPACE) {
			for (auto c : token.value) {
				if (c == '\t') {
					std::wcout << "    ";
				}
				else {
					std::wcout << c;
				}
			}
		}
		else {
			std::wcout << token.value;
		}
	}
	std::wcout << Color(205, 205, 205); // Reset console colors
}

std::wostream& operator<<(std::wostream& stream, const dawn::Color& color)
{
	using namespace dawn;
	stream << "\033[38;2;" << Int(color.r) << ";" << Int(color.g) << ";" << Int(color.b) << "m";
	return stream;
}
