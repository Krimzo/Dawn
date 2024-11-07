#pragma once

#include "t.h"


namespace dawn {
	struct Color
	{
		uint8_t r, g, b;
	};

	std::wostream& operator<<(std::wostream& stream, const Color& color);
}

namespace dawn {
	struct ColoredText
	{
		Color color;
		String text;

		template<typename... Args>
		ColoredText(Color color, const Args&... args)
			: color(color)
		{
			StringStream stream;
			(stream << ... << args);
			text = stream.str();
		}
	};

	std::wostream& operator<<(std::wostream& stream, const ColoredText& colored_text);
}

namespace dawn {
	Char to_escaping(Char c);
	String from_escaping(Char c);
	String read_file(const StringRef& path);
}
