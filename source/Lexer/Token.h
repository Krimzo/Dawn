#pragma once

#include <string>
#include <iostream>


using int64 = int64_t;
using byte = unsigned char;

namespace Faxdawn {
	struct Token {
		enum class Type {
			Unknown = 0,
			Separator,
			Operator,
			Keyword,
			Type,
			Literal,
			Identifier
		};

		Type type = Type::Unknown;
		std::string value;
		int line = -1;

		Token();
		~Token();
	};

	std::ostream& operator<<(std::ostream& stream, const Token& token);
}
