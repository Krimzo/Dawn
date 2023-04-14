#pragma once

#include "dawn/CppTypes.h"


namespace dawn {
	enum TokenType
	{
		Unknown = 0,
		Separator,
		Operator,
		Keyword,
		Literal,
		Type,
		Identifier,
	};
}

namespace dawn {
	String GetTokenDescription(TokenType type);
}

namespace dawn {
	struct Token {
		TokenType type = Unknown;
		String value = {};
		Int line = -1;
	};
}

namespace dawn {
	std::wostream& operator<<(std::wostream& stream, const Token& token);
}
