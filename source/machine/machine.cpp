#include "machine/machine.h"

#include "faxdawn/utility.h"
#include "file_io/file_io.h"


static void debug_print_tokens(const std::vector<faxdawn::token>& tokens, const std::vector<std::string>& types)
{
	using namespace faxdawn::utility;

	// Tokens
	for (auto& token : tokens) {
		print(token);
	}

	// Types
	print<false>("Types: [");
	for (size_t i = 0; i < types.size() - 1; i++) {
		print<false>(types[i], ", ");
	}
	print(types[types.size() - 1], "]");
}

bool faxdawn::machine::compile(const std::string& source) const
{
	auto tokens = lexer_.generate(source);
	tokens = parser_.parse(tokens);

#ifndef NDEBUG
	debug_print_tokens(tokens, types_);
#endif
	
	return true;
}

bool faxdawn::machine::compile_file(const std::string& filepath) const
{
	return compile(file_io::read_string(filepath));
}

bool faxdawn::machine::run(const std::vector<token>& tokens)
{
	return false;
}

bool faxdawn::machine::run_file(const std::string& filepath)
{
	return false;
}
