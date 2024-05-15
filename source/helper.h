#pragma once

#include "lexer.h"


namespace dawn {
	struct Color
	{
		UInt8 r = 0;
		UInt8 g = 0;
		UInt8 b = 0;

		Color()
		{}

		Color(UInt8 r, UInt8 g, UInt8 b)
			: r(r), g(g), b(b)
		{}
	};
}

namespace dawn {
	Color get_token_color(const Token& token);
	void display_colored(const Array<Token>& tokens);
}

namespace dawn {
	inline String read_file(const StringView& path)
	{
		StringStream stream;
		stream << std::wifstream(path.data()).rdbuf();
		return stream.str();
	}
}

std::wostream& operator<<(std::wostream& stream, const dawn::Color& color);
