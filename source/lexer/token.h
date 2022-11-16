#pragma once

#include <string>
#include <iostream>


namespace faxdawn {
	enum class token_type {
		Unknown = 0,
		Separator,
		Operator,
		Keyword,
		Literal,
		Type,
		Identifier,
	};

	struct token {
		token_type type = token_type::Unknown;
		std::string value;
		int line = -1;

		token() = default;

		[[nodiscard]] std::string type_description() const;
	};

	std::ostream& operator<<(std::ostream& stream, const token& token);
}
