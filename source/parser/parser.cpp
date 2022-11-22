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

std::vector<faxdawn::token> faxdawn::parser::parse(const std::vector<token>& tokens) const
{
	std::vector<token> parsed_tokens = tokens;
	parsed_tokens = fix_class_types(parsed_tokens);
	parsed_tokens = fix_identifier_types(parsed_tokens);
	return parsed_tokens;
}
