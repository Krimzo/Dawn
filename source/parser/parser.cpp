#include "parser/parser.h"

#include "faxdawn/utility.h"


faxdawn::parser::parser(std::unordered_set<std::string>& types)
	: types_(types)
{}

std::vector<faxdawn::token> faxdawn::parser::fix_class_types(const std::vector<token>& tokens) const
{
	std::vector<token> updated_tokens;
	updated_tokens.reserve(tokens.size());
	bool last_was_type_decl = false;
	for (auto& token : tokens) {
		faxdawn::token updated_token = token;
		if (last_was_type_decl) {
			utility::assert(updated_token.type != token_type::Identifier, updated_token);
			types_.insert(updated_token.value);
			updated_token.type = token_type::Type;
			last_was_type_decl = false;
		}
		else if (updated_token.value == syntax::keyword::type_declaration) {
			last_was_type_decl = true;
		}
		updated_tokens.push_back(updated_token);
	}
	return updated_tokens;
}

std::vector<faxdawn::token> faxdawn::parser::fix_identifier_types(const std::vector<token>& tokens) const
{
	std::vector<token> updated_tokens;
	updated_tokens.reserve(tokens.size());
	for (auto& token : tokens) {
		faxdawn::token updated_token = token;
		if (updated_token.type == token_type::Identifier && types_.contains(updated_token.value)) {
			updated_token.type = token_type::Type;
		}
		updated_tokens.push_back(updated_token);
	}
	return updated_tokens;
}

std::vector<faxdawn::token> faxdawn::parser::fix_array_types(const std::vector<token>& tokens)
{
	std::vector<token> updated_tokens;
	updated_tokens.reserve(tokens.size());
	int state = 0;
	for (auto& token : tokens) {
		faxdawn::token update_token = token;
		bool should_save = true;
		if (state == 0) {
			if (update_token.type == token_type::Type) {
				state = 1;
			}
		}
		else if (state == 1) {
			state = update_token.value == syntax::separator::open_array ? 2 : 0;
		}
		else {
			if (update_token.value == syntax::separator::close_array) {
				updated_tokens.pop_back();
				std::string& value = updated_tokens.back().value;
				value += syntax::separator::open_array;
				value += syntax::separator::close_array;
				should_save = false;
			}
			state = 0;
		}
		if (should_save) {
			updated_tokens.push_back(update_token);
		}
	}
	return updated_tokens;
}

std::vector<faxdawn::token> faxdawn::parser::parse(const std::vector<token>& tokens) const
{
	std::vector<token> parsed_tokens = tokens;
	parsed_tokens = fix_class_types(parsed_tokens);
	parsed_tokens = fix_identifier_types(parsed_tokens);
	parsed_tokens = fix_array_types(parsed_tokens);
	return parsed_tokens;
}
