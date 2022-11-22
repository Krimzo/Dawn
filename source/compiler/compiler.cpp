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

void faxdawn::compiler::set_macro(const std::string& name, const std::string& value)
{
    preprocessor_.macros[name] = value;
}

std::string faxdawn::compiler::get_macro(const std::string& name) const
{
    if (preprocessor_.macros.contains(name)) {
        return preprocessor_.macros.at(name);
    }
    return {};
}

void faxdawn::compiler::remove_macro(const std::string& name)
{
    preprocessor_.macros.erase(name);
}

std::unordered_map<std::string, faxdawn::function> faxdawn::compiler::compile(const std::string& source) const
{
    std::string result = preprocessor_.process(source);
    std::vector<token> tokens = lexer_.generate(result);
    tokens = parser_.parse(tokens);

#ifndef NDEBUG
    debug_print_tokens(tokens, types_);
#endif

    return compile(tokens);
}

std::unordered_map<std::string, faxdawn::function> faxdawn::compiler::compile(const std::vector<token>& tokens) const
{
    return {};
}
