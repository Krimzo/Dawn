#include "compiler/Compiler.h"

#include "dawn/IO.h"


dawn::Map<dawn::String, dawn::Function> dawn::Compiler::Compile(const String& source) const {
    String result = preprocessor.Process(source);
    Array<Token> tokens = lexer.Generate(result);
    tokens = parser.Parse(tokens);
    return Compile(tokens);
}

dawn::Map<dawn::String, dawn::Function> dawn::Compiler::Compile(const Array<Token>& tokens) const {
	/* DEBUG */
	// Tokens
	for (auto& token : tokens) {
		Print(token);
	}

	// Types
	Print<false>("Types: [ ");
	for (const auto& type : types) {
		Print<false>(type, " ");
	}
	Print("]");
	/* ----- */

    return {};
}
