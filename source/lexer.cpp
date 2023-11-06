#include "lexer.h"


static std::string error_helper(const size_t line_number, const char c)
{
	std::stringstream stream = {};
	stream << "Lexer error: Unknown char ";
	if (c > 32 && c < 127) {
		stream << c;
	}
	else {
		stream << "\\" << static_cast<int>(c);
	}
	stream << " at line " << line_number;
	return stream.str();
}

std::optional<std::string> dawn::Lexer::process(const std::string_view& source, std::vector<Token>* tokens) const
{
	size_t new_line_counter = 1;
	for (size_t i = 0; i < source.size(); i++) {
		// Range check
		if (source[i] < 0 || source[i] > 127) {
			return { error_helper(new_line_counter, source[i]) };
		}

		// Whitespace
		if (isspace(source[i])) {
			Token token = {};
			token.line_number = new_line_counter;
			for (; i < source.size(); i++) {
				if (source[i] == '\n') {
					new_line_counter += 1;
				}

				if (isspace(source[i])) {
					token.value.push_back(source[i]);
				}
				else {
					break;
				}
			}
			token.type = TokenType::WHITESPACE;
			tokens->push_back(token);

			i -= 1;
			continue;
		}

		// Single line comments
		if (handle_line_comments(source, i)) {
			Token token = {};
			token.line_number = new_line_counter;
			token.value = "\n";
			token.type = TokenType::WHITESPACE;
			tokens->push_back(token);

			new_line_counter += 1;

			i -= 1;
			continue;
		}

		// Multiline comments
		if (handle_multiline_comments(source, i, new_line_counter)) {
			i -= 1;
			continue;
		}

		// Identifiers and Keywords
		if (isalpha(source[i])) {
			Token token = {};
			token.line_number = new_line_counter;
			for (; i < source.size(); i++) {
				if (isalnum(source[i]) || source[i] == identifier_extender) {
					token.value.push_back(source[i]);
				}
				else {
					break;
				}
			}
			token.type = iskey(token.value) ? TokenType::KEYWORD : TokenType::IDENTIFIER;
			tokens->push_back(token);

			i -= 1;
			continue;
		}

		// Numbers
		if (isdigit(source[i])) {
			Token token = {};
			token.line_number = new_line_counter;
			bool is_floating = false;
			for (; i < source.size(); i++) {
				const bool is_decimal_definer = (source[i] == decimal_number_definer);
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
			token.type = is_floating ? TokenType::FLOAT : TokenType::INTEGER;
			tokens->push_back(token);

			i -= 1;
			continue;
		}

		// Chars
		if (source[i] == literal_char) {
			Token token = {};
			token.line_number = new_line_counter;
			token.value.push_back(source[i++]);
			for (; i < source.size(); i++) {
				token.value.push_back(source[i]);
				if (source[i] == literal_char) {
					i += 1;
					break;
				}
			}
			token.type = TokenType::CHAR;
			tokens->push_back(token);

			i -= 1;
			continue;
		}

		// Strings
		if (source[i] == literal_string) {
			Token token = {};
			token.line_number = new_line_counter;
			token.value.push_back(source[i++]);
			for (; i < source.size(); i++) {
				token.value.push_back(source[i]);
				if (source[i] == literal_string) {
					i += 1;
					break;
				}
			}
			token.type = TokenType::STRING;
			tokens->push_back(token);

			i -= 1;
			continue;
		}

		// Operators
		if (isop(source[i])) {
			Token token = {};
			token.line_number = new_line_counter;
			for (; i < source.size(); i++) {
				token.value.push_back(source[i]);
				if (!isop(token.value)) {
					token.value.pop_back();
					break;
				}
			}
			token.type = TokenType::OPERATOR;
			tokens->push_back(token);

			i -= 1;
			continue;
		}

		// Separators
		if (issep(source[i])) {
			Token token = {};
			token.line_number = new_line_counter;
			token.value = std::string(1, source[i]);
			token.type = TokenType::SEPARATOR;
			tokens->push_back(token);

			continue;
		}

		// Error
		return { error_helper(new_line_counter, source[i]) };
	}
	return {};
}

bool dawn::Lexer::handle_line_comments(const std::string_view& source, size_t& i) const
{
	if (line_comment.contains(source[i])) {
		std::string temp_holder = {};
		for (; i < source.size(); i++) {
			temp_holder.push_back(source[i]);
			if (temp_holder == line_comment) {
				for (; i < source.size(); i++) {
					if (source[i] == '\n') {
						i += 1;
						return true;
					}
				}
			}
			if (!line_comment.contains(temp_holder)) {
				i -= temp_holder.size() - 1;
				return false;
			}
		}
	}
	return false;
}

bool dawn::Lexer::handle_multiline_comments(const std::string_view& source, size_t& i, size_t& new_line_counter) const
{
	const size_t old_new_line_counter = new_line_counter;
	if (multiline_comment.first.contains(source[i])) {
		std::string left_holder = {};
		for (; i < source.size(); i++) {
			left_holder.push_back(source[i]);
			if (left_holder == multiline_comment.first) {
				std::string right_holder = {};
				for (; i < source.size(); i++) {
					const bool had_contained = !right_holder.empty() && multiline_comment.second.contains(right_holder);
					right_holder.push_back(source[i]);
					if (right_holder == multiline_comment.second) {
						i += 1;
						return true;
					}
					if (!multiline_comment.second.contains(right_holder)) {
						right_holder.clear();
						if (had_contained) {
							i -= 1;
						}
					}
					if (source[i] == '\n') {
						new_line_counter += 1;
					}
				}
			}
			if (!multiline_comment.first.contains(left_holder)) {
				i -= left_holder.size() - 1;
				new_line_counter = old_new_line_counter;
				return false;
			}
			if (source[i] == '\n') {
				new_line_counter += 1;
			}
		}
	}
	new_line_counter = old_new_line_counter;
	return false;
}

// Keywords
bool dawn::Lexer::iskey(const std::string& data) const
{
	return keywords.contains(data);
}

// Operators
bool dawn::Lexer::isop(const std::string& data) const
{
	return operators.contains(data);
}

bool dawn::Lexer::isop(const char value) const
{
	return this->isop(std::string(1, value));
}

// Separators
bool dawn::Lexer::issep(const char value) const
{
	return separators.contains(value);
}

void dawn::Lexer::load_defualt_dawn()
{
	keywords = {
		kw_module,
		kw_import,
		kw_internal,
		kw_def,
		kw_return,
		kw_let,
		kw_var,
		kw_null,
		kw_new,
		kw_if,
		kw_else,
		kw_elif,
		kw_switch,
		kw_case,
		kw_default,
		kw_for,
		kw_while,
		kw_loop,
		kw_continue,
		kw_break,
		kw_enum,
		kw_struct,
		kw_interface,
		kw_implement,
		kw_class,
		kw_init,
		kw_deinit,
		kw_self,
		kw_int,
		kw_int8,
		kw_int16,
		kw_int32,
		kw_int64,
		kw_uint,
		kw_uint8,
		kw_uint16,
		kw_uint32,
		kw_uint64,
		kw_float,
		kw_double,
		kw_char,
		kw_string,
		kw_bool,
		kw_true,
		kw_false,
	};

	operators = {
		op_add,
		op_sub,
		op_mul,
		op_div,
		op_pow,
		op_mod,
		op_add_eq,
		op_sub_eq,
		op_mul_eq,
		op_div_eq,
		op_pow_eq,
		op_mod_eq,
		op_and,
		op_or,
		op_not,
		op_less,
		op_great,
		op_eq,
		op_not_eq,
		op_less_eq,
		op_great_eq,
		op_ptr_access,
	};

	separators = {
		sep_assign,
		sep_split,
		sep_express,
		sep_access,
		sep_static_access,
		sep_def_open,
		sep_def_close,
		sep_scope_open,
		sep_scope_close,
		sep_array_open,
		sep_array_close,
	};

	identifier_extender = misc_iden_ex;
	decimal_number_definer = misc_dec_ex;

	literal_char = misc_lit_char;
	literal_string = misc_lit_str;

	line_comment = misc_line_comm;
	multiline_comment = misc_multiline_comm;
}

std::ostream& operator<<(std::ostream& stream, const dawn::TokenType type)
{
	switch (type) {
	case dawn::TokenType::WHITESPACE:
		stream << "Whitespace";
		break;
	case dawn::TokenType::IDENTIFIER:
		stream << "Identifier";
		break;
	case dawn::TokenType::KEYWORD:
		stream << "Keyword";
		break;
	case dawn::TokenType::INTEGER:
		stream << "Integer";
		break;
	case dawn::TokenType::FLOAT:
		stream << "Float";
		break;
	case dawn::TokenType::CHAR:
		stream << "Char";
		break;
	case dawn::TokenType::STRING:
		stream << "String";
		break;
	case dawn::TokenType::OPERATOR:
		stream << "Operator";
		break;
	case dawn::TokenType::SEPARATOR:
		stream << "Separator";
		break;
	}
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const dawn::Token& token)
{
	stream << '[';
	if (token.type == dawn::TokenType::WHITESPACE) {
		for (size_t i = 0; i < token.value.size() - 1; i++) {
			stream << "'\\" << static_cast<int>(token.value[i]) << "' ";
		}
		if (!token.value.empty()) {
			stream << "'\\" << static_cast<int>(*--token.value.end()) << "'";
		}
	}
	else {
		stream << token.value;
	}
	stream << ", " << token.type << ", {" << token.line_number << "}]";
	return stream;
}
