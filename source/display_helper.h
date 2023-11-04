#pragma once

#include "lexer.h"


namespace dawn {
	struct Color
	{
		uint8_t r = 0;
		uint8_t g = 0;
		uint8_t b = 0;

		Color()
		{}

		Color(uint8_t r, uint8_t g, uint8_t b)
			: r(r), g(g), b(b)
		{}
	};
}

namespace dawn {
	Color get_token_color(const Token& token);
	void display_colored(const std::vector<Token>& tokens);
}

std::ostream& operator<<(std::ostream& stream, const dawn::Color& color);
