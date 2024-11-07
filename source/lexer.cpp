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

std::wostream& dawn::operator<<(std::wostream& stream, const TokenType type)
{
	switch (type)
	{
	case TokenType::IDENTIFIER: stream << "Identifier"; break;
	case TokenType::KEYWORD: stream << "Keyword"; break;
	case TokenType::INT: stream << "Int"; break;
	case TokenType::FLOAT: stream << "Float"; break;
	case TokenType::CHAR: stream << "Char"; break;
	case TokenType::STRING: stream << "String"; break;
	case TokenType::OPERATOR: stream << "Operator"; break;
	case TokenType::SEPARATOR: stream << "Separator"; break;
	}
	return stream;
}

std::wostream& dawn::operator<<(std::wostream& stream, const Token& token)
{
	stream << "[value:" << token.value << ", type:" << token.type << ", line:" << token.line_number << "]";
	return stream;
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
		(String) kw_yield,
		(String) kw_let,
		(String) kw_var,
		(String) kw_null,
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
		(String) kw_impl,
		(String) kw_class,
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
	};
	result.separators = {
		(String) sep_assign,
		(String) sep_split,
		(String) sep_end,
		(String) sep_access,
		(String) sep_static_access,
		(String) sep_expr_open,
		(String) sep_expr_close,
		(String) sep_scope_open,
		(String) sep_scope_close,
		(String) sep_array_open,
		(String) sep_array_close,
		(String) sep_yield_open,
		(String) sep_yield_close,
	};
	result.identifier_separator = misc_iden_sep;
	result.decimal_separator = misc_dec_sep;
	result.literal_char = misc_lit_char;
	result.literal_string = misc_lit_str;
	result.line_comment = misc_line_comm;
	result.multiline_comment = misc_multiline_comm;
	return result;
}

dawn::Opt<dawn::String> dawn::Lexer::tokenize(const StringRef& source, Array<Token>& tokens)
{
	Int lines = 1;
	for (Int i = 0; i < (Int) source.size(); i++) {
		if (is_space(source, i)) {
			extract_space(source, tokens, lines, i);
		}
		else if (is_comment(source, i)) {
			extract_comment(source, tokens, lines, i);
		}
		else if (is_mlcomment(source, i)) {
			extract_mlcomment(source, tokens, lines, i);
		}
		else if (is_word(source, i)) {
			extract_word(source, tokens, lines, i);
		}
		else if (is_number(source, i)) {
			extract_number(source, tokens, lines, i);
		}
		else if (is_char(source, i)) {
			extract_char(source, tokens, lines, i);
		}
		else if (is_string(source, i)) {
			extract_string(source, tokens, lines, i);
		}
		else if (is_operator(source, i)) {
			extract_operator(source, tokens, lines, i);
		}
		else if (is_separator(source, i)) {
			extract_separator(source, tokens, lines, i);
		}
		else {
			return { error_helper(lines, source[i]) };
		}
	}
	return std::nullopt;

	//else if (handle_multiline_comments(source, i, line_counter)) {
	//	i -= 1;
	//}
	//else if (isalpha(source[i]) || source[i] == lang_def.identifier_extender) {
	//	Token token;
	//	token.line_number = line_counter;
	//	for (; i < (Int) source.size(); i++) {
	//		if (isalnum(source[i]) || source[i] == lang_def.identifier_extender) {
	//			token.value.push_back(source[i]);
	//		}
	//		else {
	//			break;
	//		}
	//	}
	//	token.type = iskey(token.value) ? TokenType::KEYWORD : TokenType::IDENTIFIER;
	//	tokens.push_back(token);
	//	i -= 1;
	//}
	//else if (isdigit(source[i])) {
	//	Token token;
	//	token.line_number = line_counter;
	//	Bool is_floating = false;
	//	for (; i < (Int) source.size(); i++) {
	//		const Bool is_decimal_definer = (source[i] == lang_def.decimal_separator);
	//		if (isdigit(source[i]) || (!is_floating && is_decimal_definer)) {
	//			token.value.push_back(source[i]);
	//		}
	//		else {
	//			break;
	//		}
	//		if (is_decimal_definer) {
	//			is_floating = true;
	//		}
	//	}
	//	token.type = is_floating ? TokenType::FLOAT : TokenType::INT;
	//	tokens.push_back(token);
	//	i -= 1;
	//}
	//else if (source[i] == lang_def.literal_char) {
	//	Token token;
	//	token.line_number = line_counter;
	//	token.value.push_back(source[i++]);
	//	for (; i < (Int) source.size(); i++) {
	//		token.value.push_back(source[i]);
	//		if (source[i] == lang_def.literal_char) {
	//			i += 1;
	//			break;
	//		}
	//	}
	//	token.type = TokenType::CHAR;
	//	tokens.push_back(token);
	//	i -= 1;
	//}
	//else if (source[i] == lang_def.literal_string) {
	//	Token token;
	//	token.line_number = line_counter;
	//	token.value.push_back(source[i++]);
	//	for (; i < (Int) source.size(); i++) {
	//		token.value.push_back(source[i]);
	//		if (source[i] == lang_def.literal_string) {
	//			i += 1;
	//			break;
	//		}
	//	}
	//	token.type = TokenType::STRING;
	//	tokens.push_back(token);
	//	i -= 1;
	//}
	//else if (isop(String(1, source[i]))) {
	//	Token token;
	//	token.line_number = line_counter;
	//	for (; i < (Int) source.size(); i++) {
	//		token.value.push_back(source[i]);
	//		if (!isop(token.value)) {
	//			token.value.pop_back();
	//			break;
	//		}
	//	}
	//	token.type = TokenType::OPERATOR;
	//	tokens.push_back(token);
	//	i -= 1;
	//}
	//else if (issep(String(1, source[i]))) {
	//	Token token;
	//	token.line_number = line_counter;
	//	token.value = String(1, source[i]);
	//	token.type = TokenType::SEPARATOR;
	//	tokens.push_back(token);
	//}
}

dawn::Bool dawn::Lexer::is_space(const StringRef& source, const Int i)
{
	return iswspace(source[i]);
}

void dawn::Lexer::extract_space(const StringRef& source, Array<Token>& tokens, Int& lines, Int& i)
{
	for (; i < (Int) source.size(); i++) {
		if (!is_space(source, i)) {
			i -= 1;
			break;
		}
		else if (source[i] == '\n') {
			lines += 1;
		}
	}
}

dawn::Bool dawn::Lexer::is_comment(const StringRef& source, const Int i)
{
	assert(false && "not implemented");
	return false;
}

void dawn::Lexer::extract_comment(const StringRef& source, Array<Token>& tokens, Int& lines, Int& i)
{
	assert(false && "not implemented");
}

dawn::Bool dawn::Lexer::is_mlcomment(const StringRef& source, const Int i)
{
	assert(false && "not implemented");
	return false;
}

void dawn::Lexer::extract_mlcomment(const StringRef& source, Array<Token>& tokens, Int& lines, Int& i)
{
	assert(false && "not implemented");
}

dawn::Bool dawn::Lexer::is_word(const StringRef& source, const Int i)
{
	assert(false && "not implemented");
	return false;
}

void dawn::Lexer::extract_word(const StringRef& source, Array<Token>& tokens, Int& lines, Int& i)
{
	assert(false && "not implemented");
}

dawn::Bool dawn::Lexer::is_number(const StringRef& source, const Int i)
{
	assert(false && "not implemented");
	return false;
}

void dawn::Lexer::extract_number(const StringRef& source, Array<Token>& tokens, Int& lines, Int& i)
{
	assert(false && "not implemented");
}

dawn::Bool dawn::Lexer::is_char(const StringRef& source, const Int i)
{
	assert(false && "not implemented");
	return false;
}

void dawn::Lexer::extract_char(const StringRef& source, Array<Token>& tokens, Int& lines, Int& i)
{
	assert(false && "not implemented");
}

dawn::Bool dawn::Lexer::is_string(const StringRef& source, const Int i)
{
	assert(false && "not implemented");
	return false;
}

void dawn::Lexer::extract_string(const StringRef& source, Array<Token>& tokens, Int& lines, Int& i)
{
	assert(false && "not implemented");
}

dawn::Bool dawn::Lexer::is_operator(const StringRef& data, const Int i)
{
	assert(false && "not implemented");
	return false;
}

void dawn::Lexer::extract_operator(const StringRef& data, Array<Token>& tokens, Int& lines, Int& i)
{
	assert(false && "not implemented");
}

dawn::Bool dawn::Lexer::is_separator(const StringRef& value, const Int i)
{
	assert(false && "not implemented");
	return false;
}

void dawn::Lexer::extract_separator(const StringRef& value, Array<Token>& tokens, Int& lines, Int& i)
{
	assert(false && "not implemented");
}

//dawn::Bool dawn::Lexer::iskey(const StringRef& data)
//{
//	return lang_def.keywords.contains((String) data);
//}
//
//dawn::Bool dawn::Lexer::isop(const StringRef& data)
//{
//	return lang_def.operators.contains((String) data);
//}
//
//dawn::Bool dawn::Lexer::issep(const StringRef& value)
//{
//	return lang_def.separators.contains((String) value);
//}
//
//bool dawn::operator==(const Char lhs, const StringRef& rhs)
//{
//	if (rhs.size() != 1)
//		return false;
//	return lhs == rhs[0];
//}
//
//dawn::Bool dawn::Lexer::handle_line_comments(const StringRef& source, Int& i)
//{
//	if (lang_def.line_comment.starts_with(source[i])) {
//		String temp_holder;
//		for (; i < (Int) source.size(); i++) {
//			temp_holder.push_back(source[i]);
//			if (temp_holder == lang_def.line_comment) {
//				for (; i < (Int) source.size(); i++) {
//					if (source[i] == '\n') {
//						i += 1;
//						return true;
//					}
//				}
//			}
//			if (!lang_def.line_comment.starts_with(temp_holder)) {
//				i -= temp_holder.size() - 1;
//				return false;
//			}
//		}
//	}
//	return false;
//}
//
//dawn::Bool dawn::Lexer::handle_multiline_comments(const StringRef& source, Int& i, Int& line_counter)
//{
//	const Int old_line_counter = line_counter;
//	if (lang_def.multiline_comment.first.contains(source[i])) {
//		String left_holder;
//		for (; i < (Int) source.size(); i++) {
//			left_holder.push_back(source[i]);
//			if (left_holder == lang_def.multiline_comment.first) {
//				String right_holder;
//				for (; i < (Int) source.size(); i++) {
//					const Bool had_contained = !right_holder.empty() && lang_def.multiline_comment.second.contains(right_holder);
//					right_holder.push_back(source[i]);
//					if (right_holder == lang_def.multiline_comment.second) {
//						i += 1;
//						return true;
//					}
//					if (!lang_def.multiline_comment.second.contains(right_holder)) {
//						right_holder.clear();
//						if (had_contained) {
//							i -= 1;
//						}
//					}
//					if (source[i] == '\n') {
//						line_counter += 1;
//					}
//				}
//			}
//			if (!lang_def.multiline_comment.first.contains(left_holder)) {
//				i -= left_holder.size() - 1;
//				line_counter = old_line_counter;
//				return false;
//			}
//			if (source[i] == '\n') {
//				line_counter += 1;
//			}
//		}
//	}
//	line_counter = old_line_counter;
//	return false;
//}
