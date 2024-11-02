#pragma once

#include "t.h"


namespace dawn {
	inline constexpr StringView kw_module = L"module";
	inline constexpr StringView kw_import = L"import";
	inline constexpr StringView kw_internal = L"internal";
	inline constexpr StringView kw_func = L"func";
	inline constexpr StringView kw_return = L"return";
	inline constexpr StringView kw_let = L"let";
	inline constexpr StringView kw_var = L"var";
	inline constexpr StringView kw_null = L"null";
	inline constexpr StringView kw_new = L"new";
	inline constexpr StringView kw_if = L"if";
	inline constexpr StringView kw_else = L"else";
	inline constexpr StringView kw_elif = L"elif";
	inline constexpr StringView kw_switch = L"switch";
	inline constexpr StringView kw_case = L"case";
	inline constexpr StringView kw_default = L"default";
	inline constexpr StringView kw_for = L"for";
	inline constexpr StringView kw_while = L"while";
	inline constexpr StringView kw_loop = L"loop";
	inline constexpr StringView kw_continue = L"continue";
	inline constexpr StringView kw_break = L"break";
	inline constexpr StringView kw_enum = L"enum";
	inline constexpr StringView kw_struct = L"struct";
	inline constexpr StringView kw_interface = L"interface";
	inline constexpr StringView kw_implement = L"implement";
	inline constexpr StringView kw_class = L"class";
	inline constexpr StringView kw_init = L"init";
	inline constexpr StringView kw_deinit = L"deinit";
	inline constexpr StringView kw_self = L"self";
	inline constexpr StringView kw_int = L"int";
	inline constexpr StringView kw_float = L"float";
	inline constexpr StringView kw_char = L"char";
	inline constexpr StringView kw_string = L"string";
	inline constexpr StringView kw_bool = L"bool";
	inline constexpr StringView kw_true = L"true";
	inline constexpr StringView kw_false = L"false";
}

namespace dawn {
	inline constexpr StringView op_add = L"+";
	inline constexpr StringView op_sub = L"-";
	inline constexpr StringView op_mul = L"*";
	inline constexpr StringView op_div = L"/";
	inline constexpr StringView op_pow = L"^";
	inline constexpr StringView op_mod = L"%";
	inline constexpr StringView op_add_as = L"+=";
	inline constexpr StringView op_sub_as = L"-=";
	inline constexpr StringView op_mul_as = L"*=";
	inline constexpr StringView op_div_as = L"/=";
	inline constexpr StringView op_pow_as = L"^=";
	inline constexpr StringView op_mod_as = L"%=";
	inline constexpr StringView op_not = L"!";
	inline constexpr StringView op_and = L"&&";
	inline constexpr StringView op_or = L"||";
	inline constexpr StringView op_eq = L"==";
	inline constexpr StringView op_not_eq = L"!=";
	inline constexpr StringView op_less = L"<";
	inline constexpr StringView op_great = L">";
	inline constexpr StringView op_less_eq = L"<=";
	inline constexpr StringView op_great_eq = L">=";
	inline constexpr StringView op_address = L"&";
	inline constexpr StringView op_access = L"->";
}

namespace dawn {
	inline constexpr StringView sep_assign = L"=";
	inline constexpr StringView sep_split = L",";
	inline constexpr StringView sep_express = L";";
	inline constexpr StringView sep_access = L".";
	inline constexpr StringView sep_static_access = L":";
	inline constexpr StringView sep_def_open = L"(";
	inline constexpr StringView sep_def_close = L")";
	inline constexpr StringView sep_scope_open = L"{";
	inline constexpr StringView sep_scope_close = L"}";
	inline constexpr StringView sep_array_open = L"[";
	inline constexpr StringView sep_array_close = L"]";
}

namespace dawn {
	inline constexpr StringView misc_iden_ex = L"_";
	inline constexpr StringView misc_dec_ex = L".";
	inline constexpr StringView misc_lit_char = L"'";
	inline constexpr StringView misc_lit_str = L"\"";
	inline constexpr StringView misc_line_comm = L"//";
	inline constexpr Pair<StringView, StringView> misc_multiline_comm = { L"/*", L"*/" };
}

namespace dawn {
	inline const Map<String, Int> precedences = {
		{ (String) sep_static_access, 1 },
		{ (String) op_access, 2 },
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
