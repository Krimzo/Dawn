#pragma once

#include "lexer/lexer.h"
#include "parser/parser.h"
#include "logic/function.h"

#include <unordered_map>


namespace faxdawn {
	class machine
	{
		std::unordered_map<std::string, function> functions_ = {};

	public:
		machine() = default;

		void load(const std::string& name, const function& function);

		void remove(const std::string& name);

		bool execute(const std::string& name) const;

		bool execute_main() const;
	};
}
