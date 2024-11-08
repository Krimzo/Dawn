#pragma once

#include "util.h"


namespace dawn {
	inline constexpr StringRef kw_module = L"module";
	inline constexpr StringRef kw_import = L"import";
	inline constexpr StringRef kw_internal = L"internal";
	inline constexpr StringRef kw_func = L"func";
	inline constexpr StringRef kw_return = L"return";
	inline constexpr StringRef kw_yield = L"yield";
	inline constexpr StringRef kw_let = L"let";
	inline constexpr StringRef kw_var = L"var";
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
	inline constexpr StringRef kw_layer = L"layer";
	inline constexpr StringRef kw_struct = L"struct";
	inline constexpr StringRef kw_impl = L"impl";
	inline constexpr StringRef kw_self = L"self";
	inline constexpr StringRef kw_bool = L"bool";
	inline constexpr StringRef kw_int = L"int";
	inline constexpr StringRef kw_float = L"float";
	inline constexpr StringRef kw_char = L"char";
	inline constexpr StringRef kw_string = L"string";
	inline constexpr StringRef kw_true = L"true";
	inline constexpr StringRef kw_false = L"false";
	inline constexpr StringRef kw_null = L"null";
}

namespace dawn {
	inline constexpr StringRef op_add = L"+";
	inline constexpr StringRef op_sub = L"-";
	inline constexpr StringRef op_mul = L"*";
	inline constexpr StringRef op_div = L"/";
	inline constexpr StringRef op_pow = L"^";
	inline constexpr StringRef op_mod = L"%";
	inline constexpr StringRef op_addas = L"+=";
	inline constexpr StringRef op_subas = L"-=";
	inline constexpr StringRef op_mulas = L"*=";
	inline constexpr StringRef op_divas = L"/=";
	inline constexpr StringRef op_powas = L"^=";
	inline constexpr StringRef op_modas = L"%=";
	inline constexpr StringRef op_not = L"!";
	inline constexpr StringRef op_and = L"&&";
	inline constexpr StringRef op_or = L"||";
	inline constexpr StringRef op_eq = L"==";
	inline constexpr StringRef op_neq = L"!=";
	inline constexpr StringRef op_less = L"<";
	inline constexpr StringRef op_great = L">";
	inline constexpr StringRef op_lesseq = L"<=";
	inline constexpr StringRef op_greateq = L">=";
	inline constexpr StringRef op_assign = L"=";
	inline constexpr StringRef op_split = L",";
	inline constexpr StringRef op_ref = L"&";
	inline constexpr StringRef op_access = L"->";
	inline constexpr StringRef op_link = L":";
	inline constexpr StringRef op_range = L"~";
	inline constexpr StringRef op_unknown = L"?";
	inline constexpr StringRef op_expr_opn = L"(";
	inline constexpr StringRef op_expr_cls = L")";
	inline constexpr StringRef op_scope_opn = L"{";
	inline constexpr StringRef op_scope_cls = L"}";
	inline constexpr StringRef op_array_opn = L"[";
	inline constexpr StringRef op_array_cls = L"]";
	inline constexpr StringRef op_yield_opn = L"<<";
	inline constexpr StringRef op_yield_cls = L">>";
	inline constexpr StringRef op_expr_end = L";";
}

namespace dawn {
	inline constexpr StringRef sep_number = L".";
	inline constexpr StringRef sep_identifier = L"_";
	inline constexpr StringRef lit_char = L"'";
	inline constexpr StringRef lit_string = L"\"";
	inline constexpr StringRef comm_line = L"//";
	inline constexpr Pair<StringRef, StringRef> comm_multiline = { L"/*", L"*/" };
}

namespace dawn {
	inline const Map<String, Int> precedences
	{
		{ (String) op_link, 1 },
		{ (String) op_access, 2 },
		{ (String) op_ref, 3 },
		{ (String) op_not, 3 },
		{ (String) op_pow, 4 },
		{ (String) op_mod, 4 },
		{ (String) op_mul, 5 },
		{ (String) op_div, 5 },
		{ (String) op_add, 6 },
		{ (String) op_sub, 6 },
		{ (String) op_less, 7 },
		{ (String) op_great, 7 },
		{ (String) op_lesseq, 7 },
		{ (String) op_greateq, 7 },
		{ (String) op_eq, 8 },
		{ (String) op_neq, 8 },
		{ (String) op_and, 9 },
		{ (String) op_or, 10 },
		{ (String) op_range, 11 },
		{ (String) op_assign, 12 },
		{ (String) op_addas, 12 },
		{ (String) op_subas, 12 },
		{ (String) op_mulas, 12 },
		{ (String) op_divas, 12 },
		{ (String) op_powas, 12 },
		{ (String) op_modas, 12 },
		{ (String) op_split, 13 },
	};
}
