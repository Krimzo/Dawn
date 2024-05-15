#pragma once

#include "t.h"


namespace dawn {
	// Modules
	inline const String kw_module = L"module";
	inline const String kw_import = L"import";
	inline const String kw_internal = L"internal";

	// Functions
	inline const String kw_def = L"def";
	inline const String kw_return = L"return";

	// Variables
	inline const String kw_let = L"let";
	inline const String kw_var = L"var";
	inline const String kw_null = L"null";
	inline const String kw_new = L"new";

	// If
	inline const String kw_if = L"if";
	inline const String kw_else = L"else";
	inline const String kw_elif = L"elif";

	// Switch
	inline const String kw_switch = L"switch";
	inline const String kw_case = L"case";
	inline const String kw_default = L"default";

	// Loops
	inline const String kw_for = L"for";
	inline const String kw_while = L"while";
	inline const String kw_loop = L"loop";
	inline const String kw_continue = L"continue";
	inline const String kw_break = L"break";

	// POD objects
	inline const String kw_enum = L"enum";
	inline const String kw_struct = L"struct";

	// Class objects
	inline const String kw_interface = L"interface";
	inline const String kw_implement = L"implement";
	inline const String kw_class = L"class";
	inline const String kw_init = L"init";
	inline const String kw_deinit = L"deinit";
	inline const String kw_self = L"self";

	// Integers
	inline const String kw_int = L"int";
	inline const String kw_int8 = L"int8";
	inline const String kw_int16 = L"int16";
	inline const String kw_int32 = L"int32";
	inline const String kw_int64 = L"int64";

	// Unsigned integers
	inline const String kw_uint = L"uint";
	inline const String kw_uint8 = L"uint8";
	inline const String kw_uint16 = L"uint16";
	inline const String kw_uint32 = L"uint32";
	inline const String kw_uint64 = L"uint64";

	// Floating point
	inline const String kw_float = L"float";
	inline const String kw_float32 = L"float32";
	inline const String kw_float64 = L"float64";

	// Text
	inline const String kw_char = L"char";
	inline const String kw_string = L"string";

	// Bools
	inline const String kw_bool = L"bool";
	inline const String kw_true = L"true";
	inline const String kw_false = L"false";
}

namespace dawn {
	// Math
	inline const String op_add = L"+";
	inline const String op_sub = L"-";
	inline const String op_mul = L"*";
	inline const String op_div = L"/";
	inline const String op_pow = L"^";
	inline const String op_mod = L"%";
	inline const String op_add_as = L"+=";
	inline const String op_sub_as = L"-=";
	inline const String op_mul_as = L"*=";
	inline const String op_div_as = L"/=";
	inline const String op_pow_as = L"%=";
	inline const String op_mod_as = L"^=";

	// Bools
	inline const String op_and = L"&&";
	inline const String op_or = L"||";
	inline const String op_not = L"!";
	inline const String op_less = L"<";
	inline const String op_great = L">";
	inline const String op_eq = L"==";
	inline const String op_not_eq = L"!=";
	inline const String op_less_eq = L"<=";
	inline const String op_great_eq = L">=";

	// Access
	inline const String op_address = L"&";
	inline const String op_access = L"->";
}

namespace dawn {
	// Access
	inline constexpr Char sep_assign = '=';
	inline constexpr Char sep_split = ',';
	inline constexpr Char sep_express = ';';
	inline constexpr Char sep_access = '.';
	inline constexpr Char sep_static_access = ':';

	// Control
	inline constexpr Char sep_def_open = '(';
	inline constexpr Char sep_def_close = ')';
	inline constexpr Char sep_scope_open = '{';
	inline constexpr Char sep_scope_close = '}';
	inline constexpr Char sep_array_open = '[';
	inline constexpr Char sep_array_close = ']';
}

namespace dawn {
	inline constexpr Char misc_iden_ex = '_';
	inline constexpr Char misc_dec_ex = '.';

	inline constexpr Char misc_lit_char = '\'';
	inline constexpr Char misc_lit_str = '"';

	inline const String misc_line_comm = L"//";
	inline const Pair<String, String> misc_multiline_comm = { L"/*", L"*/" };
}

namespace dawn {
	inline const Map<String, Int> precedences = {
		{ String(1, sep_static_access), 1 },
		{ op_access, 2 }, /* CASTING, FUNCTION CALLS, COLLECTION ACCESS */
		{ op_address, 3 }, /* UNARY +-, DEREFERENCE */
		{ op_not, 3 },
		{ op_pow, 4 },
		{ op_mod, 4 },
		{ op_mul, 5 },
		{ op_div, 5 },
		{ op_add, 6 },
		{ op_sub, 6 },
		{ op_less, 7 },
		{ op_great, 7 },
		{ op_less_eq, 7 },
		{ op_great_eq, 7 },
		{ op_eq, 8 },
		{ op_not_eq, 8 },
		{ op_and, 9 },
		{ op_or, 9 },
		{ String(1, sep_assign), 10 },
		{ op_add_as, 10 },
		{ op_sub_as, 10 },
		{ op_mul_as, 10 },
		{ op_div_as, 10 },
		{ op_pow_as, 10 },
		{ op_mod_as, 10 },
		{ String(1, sep_split), 11 },
	};
}
