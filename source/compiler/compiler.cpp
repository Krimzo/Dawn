#include "compiler/compiler.h"

#include "faxdawn/utility.h"
#include "lexer/token.h"


static void debug_print_tokens(const std::vector<faxdawn::token>& tokens, const std::unordered_set<std::string>& types)
{
	using namespace faxdawn::utility;

	// Tokens
	for (auto& token : tokens) {
		print(token);
	}

	// Types
	print<false>("Types: [ ");
	for (auto& val : types) {
		print<false>(val, " ");
	}
	print("]");
}

std::vector<faxdawn::function> faxdawn::compiler::compile(const std::string& source) const
{
    std::string result = preprocessor_.process(source);
    std::vector<token> tokens = lexer_.generate(result);
    tokens = parser_.parse(tokens);

#ifdef _DEBUG
    debug_print_tokens(tokens, types_);
#endif

    return compile(tokens);
}

std::vector<faxdawn::function> faxdawn::compiler::compile(const std::vector<token>& tokens) const
{
    return {};
}
