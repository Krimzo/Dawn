#include "lexer.h"


static dawn::String error_helper(const dawn::Int line_number, const dawn::Char c)
{
	using namespace dawn;

	StringStream stream;
	stream << "Lexer error: Unknown char ";
	if (c > 32 && c < 127) {
		stream << c;
	}
	else {
		stream << "\\" << Int(c);
	}
	stream << " at line " << line_number;
	return stream.str();
}

dawn::LanguageDef dawn::LanguageDef::default_def()
{
	LanguageDef result;
	result.keywords = {
		(String) kw_module,
		(String) kw_import,
		(String) kw_internal,
		(String) kw_func,
		(String) kw_return,
		(String) kw_let,
		(String) kw_var,
		(String) kw_null,
		(String) kw_new,
		(String) kw_if,
		(String) kw_else,
		(String) kw_elif,
		(String) kw_switch,
		(String) kw_case,
		(String) kw_default,
		(String) kw_for,
		(String) kw_while,
		(String) kw_loop,
		(String) kw_continue,
		(String) kw_break,
		(String) kw_enum,
		(String) kw_struct,
		(String) kw_interface,
		(String) kw_implement,
		(String) kw_class,
		(String) kw_init,
		(String) kw_deinit,
		(String) kw_self,
		(String) kw_int,
		(String) kw_float,
		(String) kw_char,
		(String) kw_string,
		(String) kw_bool,
		(String) kw_true,
		(String) kw_false,
	};
	result.operators = {
		(String) op_add,
		(String) op_sub,
		(String) op_mul,
		(String) op_div,
		(String) op_pow,
		(String) op_mod,
		(String) op_add_as,
		(String) op_sub_as,
		(String) op_mul_as,
		(String) op_div_as,
		(String) op_pow_as,
		(String) op_mod_as,
		(String) op_not,
		(String) op_and,
		(String) op_or,
		(String) op_eq,
		(String) op_not_eq,
		(String) op_less,
		(String) op_great,
		(String) op_less_eq,
		(String) op_great_eq,
		(String) op_address,
		(String) op_access,
	};
	result.separators = {
		(String) sep_assign,
		(String) sep_split,
		(String) sep_express,
		(String) sep_access,
		(String) sep_static_access,
		(String) sep_def_open,
		(String) sep_def_close,
		(String) sep_scope_open,
		(String) sep_scope_close,
		(String) sep_array_open,
		(String) sep_array_close,
	};
	result.identifier_extender = misc_iden_ex;
	result.decimal_separator = misc_dec_ex;
	result.literal_char = misc_lit_char;
	result.literal_string = misc_lit_str;
	result.line_comment = misc_line_comm;
	result.multiline_comment = misc_multiline_comm;
	return result;
}

dawn::Opt<dawn::String> dawn::Lexer::tokenize(const StringView& source, Array<Token>& tokens) const
{
	Int line_counter = 1;
	for (Int i = 0; i < (Int) source.size(); i++) {
		if (source[i] < 0 || source[i] > 127)
			return { error_helper(line_counter, source[i]) };

		if (isspace(source[i])) {
			for (; i < (Int) source.size(); i++) {
				if (source[i] == '\n') {
					line_counter += 1;
				}
				if (!isspace(source[i]))
					break;
			}
			i -= 1;
			continue;
		}

		if (handle_line_comments(source, i)) {
			line_counter += 1;
			i -= 1;
			continue;
		}

		if (handle_multiline_comments(source, i, line_counter)) {
			i -= 1;
			continue;
		}

		if (isalpha(source[i])) {
			Token token;
			token.line_number = line_counter;
			for (; i < (Int) source.size(); i++) {
				if (isalnum(source[i]) || source[i] == lang_def.identifier_extender) {
					token.value.push_back(source[i]);
				}
				else {
					break;
				}
			}
			token.type = iskey(token.value) ? TokenType::KEYWORD : TokenType::IDENTIFIER;
			tokens.push_back(token);
			i -= 1;
			continue;
		}

		if (isdigit(source[i])) {
			Token token;
			token.line_number = line_counter;
			Bool is_floating = false;
			for (; i < (Int) source.size(); i++) {
				const Bool is_decimal_definer = (source[i] == lang_def.decimal_separator);
				if (isdigit(source[i]) || (!is_floating && is_decimal_definer)) {
					token.value.push_back(source[i]);
				}
				else {
					break;
				}
				if (is_decimal_definer) {
					is_floating = true;
				}
			}
			token.type = is_floating ? TokenType::FLOAT : TokenType::INT;
			tokens.push_back(token);
			i -= 1;
			continue;
		}

		if (source[i] == lang_def.literal_char) {
			Token token = {};
			token.line_number = line_counter;
			token.value.push_back(source[i++]);
			for (; i < (Int) source.size(); i++) {
				token.value.push_back(source[i]);
				if (source[i] == lang_def.literal_char) {
					i += 1;
					break;
				}
			}
			token.type = TokenType::CHAR;
			tokens.push_back(token);
			i -= 1;
			continue;
		}

		if (source[i] == lang_def.literal_string) {
			Token token = {};
			token.line_number = line_counter;
			token.value.push_back(source[i++]);
			for (; i < (Int) source.size(); i++) {
				token.value.push_back(source[i]);
				if (source[i] == lang_def.literal_string) {
					i += 1;
					break;
				}
			}
			token.type = TokenType::STRING;
			tokens.push_back(token);
			i -= 1;
			continue;
		}

		if (isop(String(1, source[i]))) {
			Token token = {};
			token.line_number = line_counter;
			for (; i < (Int) source.size(); i++) {
				token.value.push_back(source[i]);
				if (!isop(token.value)) {
					token.value.pop_back();
					break;
				}
			}
			token.type = TokenType::OPERATOR;
			tokens.push_back(token);
			i -= 1;
			continue;
		}

		if (issep(String(1, source[i]))) {
			Token token = {};
			token.line_number = line_counter;
			token.value = String(1, source[i]);
			token.type = TokenType::SEPARATOR;
			tokens.push_back(token);
			continue;
		}

		return { error_helper(line_counter, source[i]) };
	}
	return {};
}

dawn::Bool dawn::Lexer::handle_line_comments(const StringView& source, Int& i) const
{
	if (lang_def.line_comment.contains(source[i])) {
		String temp_holder = {};
		for (; i < (Int) source.size(); i++) {
			temp_holder.push_back(source[i]);
			if (temp_holder == lang_def.line_comment) {
				for (; i < (Int) source.size(); i++) {
					if (source[i] == '\n') {
						i += 1;
						return true;
					}
				}
			}
			if (!lang_def.line_comment.contains(temp_holder)) {
				i -= temp_holder.size() - 1;
				return false;
			}
		}
	}
	return false;
}

dawn::Bool dawn::Lexer::handle_multiline_comments(const StringView& source, Int& i, Int& line_counter) const
{
	const Int old_line_counter = line_counter;
	if (lang_def.multiline_comment.first.contains(source[i])) {
		String left_holder = {};
		for (; i < (Int) source.size(); i++) {
			left_holder.push_back(source[i]);
			if (left_holder == lang_def.multiline_comment.first) {
				String right_holder = {};
				for (; i < (Int) source.size(); i++) {
					const Bool had_contained = !right_holder.empty() && lang_def.multiline_comment.second.contains(right_holder);
					right_holder.push_back(source[i]);
					if (right_holder == lang_def.multiline_comment.second) {
						i += 1;
						return true;
					}
					if (!lang_def.multiline_comment.second.contains(right_holder)) {
						right_holder.clear();
						if (had_contained) {
							i -= 1;
						}
					}
					if (source[i] == '\n') {
						line_counter += 1;
					}
				}
			}
			if (!lang_def.multiline_comment.first.contains(left_holder)) {
				i -= left_holder.size() - 1;
				line_counter = old_line_counter;
				return false;
			}
			if (source[i] == '\n') {
				line_counter += 1;
			}
		}
	}
	line_counter = old_line_counter;
	return false;
}

dawn::Bool dawn::Lexer::iskey(const StringView& data) const
{
	return lang_def.keywords.contains((String) data);
}

dawn::Bool dawn::Lexer::isop(const StringView& data) const
{
	return lang_def.operators.contains((String) data);
}

dawn::Bool dawn::Lexer::issep(const StringView& value) const
{
	return lang_def.separators.contains((String) value);
}

bool dawn::operator==(const dawn::Char lhs, const dawn::StringView& rhs)
{
	if (rhs.size() != 1)
		return false;
	return lhs == rhs[0];
}

std::wostream& dawn::operator<<(std::wostream& stream, const dawn::TokenType type)
{
	switch (type)
	{
	case dawn::TokenType::IDENTIFIER: stream << "Identifier"; break;
	case dawn::TokenType::KEYWORD: stream << "Keyword"; break;
	case dawn::TokenType::INT: stream << "Int"; break;
	case dawn::TokenType::FLOAT: stream << "Float"; break;
	case dawn::TokenType::CHAR: stream << "Char"; break;
	case dawn::TokenType::STRING: stream << "String"; break;
	case dawn::TokenType::OPERATOR: stream << "Operator"; break;
	case dawn::TokenType::SEPARATOR: stream << "Separator"; break;
	}
	return stream;
}

std::wostream& dawn::operator<<(std::wostream& stream, const dawn::Token& token)
{
	stream << '[' << token.value << ", " << token.type << ", {" << token.line_number << "}]";
	return stream;
}
