#pragma once

#include "lexer/Lexer.h"


namespace dawn {
	class Parser
	{
	public:
		Set<String>& typesRef;

		Parser(Set<String>& types);

		Array<Array<Token>> ExtractUses(const Array<Token>& tokens) const;
		Array<Array<Token>> ExtractTypes(const Array<Token>& tokens) const;
		Array<Array<Token>> ExtractGlobalVars(const Array<Token>& tokens) const;
		Array<Array<Token>> ExtractFunctions(const Array<Token>& tokens) const;

		Array<Array<Token>> ExtractFunctionCommands(const Array<Token>& tokens) const;
		Array<Array<Token>> ExtractScopeCommands(const Array<Token>& tokens) const;
	};
}
