#include "lexer/lexer.h"

#include <sstream>


faxdawn::lexer::lexer(std::vector<std::string>& types)
	: types_(types)
{}

static void save_stream_part(std::stringstream& stream, std::vector<faxdawn::token>& tokens, const int line_id)
{
	if (const std::string result = stream.str(); !faxdawn::syntax::is_discarded(result)) {
		faxdawn::token token;
		token.value = result;
		token.line = line_id;
		tokens.push_back(token);
	}
	stream = {};
}

std::vector<faxdawn::token> faxdawn::lexer::split(const std::string& source) const
{
	std::vector<token> tokens;

	bool reading_comment = false;
	bool reading_splitter = false;
	bool reading_string = false;
	std::stringstream value_stream = {};
	int line_id = 1;

	for (const auto& source_char : source) {
		if (source_char == '\n') {
			line_id += 1;
		}
		if (source_char == syntax::comment) {
			reading_comment = !reading_comment;
		}
		else if (!reading_comment && !syntax::is_ignored(source_char)) {
			if (source_char == syntax::string) {
				reading_string = !reading_string;
			}
			if (!reading_string && (syntax::is_separator(source_char) || syntax::is_operator(source_char))) {
				if (reading_splitter) {
					if (syntax::is_operator(value_stream.str() + source_char)) {
						value_stream << source_char;
						save_stream_part(value_stream, tokens, line_id);
						reading_splitter = false;
					}
					else {
						save_stream_part(value_stream, tokens, line_id);
						value_stream << source_char;
					}
				}
				else {
					save_stream_part(value_stream, tokens, line_id);
					value_stream << source_char;
					reading_splitter = true;
				}
			}
			else {
				if (reading_splitter) {
					save_stream_part(value_stream, tokens, line_id);
					reading_splitter = false;
				}
				value_stream << source_char;
			}
		}
	}
	return tokens;
}

std::vector<faxdawn::token> faxdawn::lexer::generate(const std::string& source) const
{
	auto tokens = split(source);
	for (auto& token : tokens) {
		token.type = syntax::get_token_type(types_, token.value);
	}
	return tokens;
}
