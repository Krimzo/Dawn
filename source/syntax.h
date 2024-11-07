#pragma once

#include "t.h"


namespace dawn {
	inline constexpr StringRef kw_module = L"module";
	inline constexpr StringRef kw_import = L"import";
	inline constexpr StringRef kw_internal = L"internal";
	inline constexpr StringRef kw_func = L"func";
	inline constexpr StringRef kw_return = L"return";
	inline constexpr StringRef kw_yield = L"yield";
	inline constexpr StringRef kw_let = L"let";
	inline constexpr StringRef kw_var = L"var";
	inline constexpr StringRef kw_null = L"null";
	inline constexpr StringRef kw_if = L"if";
	inline constexpr StringRef kw_else = L"else";
	inline constexpr StringRef kw_elif = L"elif";
	inline constexpr StringRef kw_switch = L"switch";
	inline constexpr StringRef kw_case = L"case";
	inline constexpr StringRef kw_default = L"default";
	inline constexpr StringRef kw_for = L"for";
	inline constexpr StringRef kw_while = L"while";
	inline constexpr StringRef kw_loop = L"loop";
	inline constexpr StringRef kw_continue = L"continue";
	inline constexpr StringRef kw_break = L"break";
	inline constexpr StringRef kw_enum = L"enum";
	inline constexpr StringRef kw_struct = L"struct";
	inline constexpr StringRef kw_interface = L"interface";
	inline constexpr StringRef kw_impl = L"impl";
	inline constexpr StringRef kw_class = L"class";
	inline constexpr StringRef kw_self = L"self";
	inline constexpr StringRef kw_bool = L"bool";
	inline constexpr StringRef kw_int = L"int";
	inline constexpr StringRef kw_float = L"float";
	inline constexpr StringRef kw_char = L"char";
	inline constexpr StringRef kw_string = L"string";
	inline constexpr StringRef kw_true = L"true";
	inline constexpr StringRef kw_false = L"false";
}

namespace dawn {
	inline constexpr StringRef op_add = L"+";
	inline constexpr StringRef op_sub = L"-";
	inline constexpr StringRef op_mul = L"*";
	inline constexpr StringRef op_div = L"/";
	inline constexpr StringRef op_pow = L"^";
	inline constexpr StringRef op_mod = L"%";
	inline constexpr StringRef op_add_as = L"+=";
	inline constexpr StringRef op_sub_as = L"-=";
	inline constexpr StringRef op_mul_as = L"*=";
	inline constexpr StringRef op_div_as = L"/=";
	inline constexpr StringRef op_pow_as = L"^=";
	inline constexpr StringRef op_mod_as = L"%=";
	inline constexpr StringRef op_not = L"!";
	inline constexpr StringRef op_and = L"&&";
	inline constexpr StringRef op_or = L"||";
	inline constexpr StringRef op_eq = L"==";
	inline constexpr StringRef op_not_eq = L"!=";
	inline constexpr StringRef op_less = L"<";
	inline constexpr StringRef op_great = L">";
	inline constexpr StringRef op_less_eq = L"<=";
	inline constexpr StringRef op_great_eq = L">=";
	inline constexpr StringRef op_address = L"&";
}

namespace dawn {
	inline constexpr StringRef sep_assign = L"=";
	inline constexpr StringRef sep_split = L",";
	inline constexpr StringRef sep_end = L";";
	inline constexpr StringRef sep_access = L".";
	inline constexpr StringRef sep_static_access = L":";
	inline constexpr StringRef sep_expr_open = L"(";
	inline constexpr StringRef sep_expr_close = L")";
	inline constexpr StringRef sep_scope_open = L"{";
	inline constexpr StringRef sep_scope_close = L"}";
	inline constexpr StringRef sep_array_open = L"[";
	inline constexpr StringRef sep_array_close = L"]";
	inline constexpr StringRef sep_yield_open = L"<<";
	inline constexpr StringRef sep_yield_close = L">>";
}

namespace dawn {
	inline constexpr StringRef misc_iden_sep = L"_";
	inline constexpr StringRef misc_dec_sep = L".";
	inline constexpr StringRef misc_lit_char = L"'";
	inline constexpr StringRef misc_lit_str = L"\"";
	inline constexpr StringRef misc_line_comm = L"//";
	inline constexpr Pair<StringRef, StringRef> misc_multiline_comm = { L"/*", L"*/" };
}

namespace dawn {
	inline const Map<String, Int> precedences
	{
		{ (String) sep_static_access, 1 },
		{ (String) sep_access, 2 },
		{ (String) op_address, 3 },
		{ (String) op_not, 3 },
		{ (String) op_pow, 4 },
		{ (String) op_mod, 4 },
		{ (String) op_mul, 5 },
		{ (String) op_div, 5 },
		{ (String) op_add, 6 },
		{ (String) op_sub, 6 },
		{ (String) op_less, 7 },
		{ (String) op_great, 7 },
		{ (String) op_less_eq, 7 },
		{ (String) op_great_eq, 7 },
		{ (String) op_eq, 8 },
		{ (String) op_not_eq, 8 },
		{ (String) op_and, 9 },
		{ (String) op_or, 10 },
		{ (String) sep_assign, 11 },
		{ (String) op_add_as, 11 },
		{ (String) op_sub_as, 11 },
		{ (String) op_mul_as, 11 },
		{ (String) op_div_as, 11 },
		{ (String) op_pow_as, 11 },
		{ (String) op_mod_as, 11 },
		{ (String) sep_split, 12 },
	};
}
