#pragma once

#include "cpp_helper.h"

namespace dawn {
	// Modules
	inline const std::string kw_module = "module";
	inline const std::string kw_import = "import";
	inline const std::string kw_internal = "internal";

	// Functions
	inline const std::string kw_def = "def";
	inline const std::string kw_return = "return";

	// Variables
	inline const std::string kw_let = "let";
	inline const std::string kw_var = "var";
	inline const std::string kw_null = "null";
	inline const std::string kw_new = "new";

	// If
	inline const std::string kw_if = "if";
	inline const std::string kw_else = "else";
	inline const std::string kw_elif = "elif";

	// Switch
	inline const std::string kw_switch = "switch";
	inline const std::string kw_case = "case";
	inline const std::string kw_default = "default";

	// Loops
	inline const std::string kw_for = "for";
	inline const std::string kw_while = "while";
	inline const std::string kw_loop = "loop";
	inline const std::string kw_continue = "continue";
	inline const std::string kw_break = "break";

	// POD objects
	inline const std::string kw_enum = "enum";
	inline const std::string kw_struct = "struct";

	// Class objects
	inline const std::string kw_interface = "interface";
	inline const std::string kw_implement = "implement";
	inline const std::string kw_class = "class";
	inline const std::string kw_init = "init";
	inline const std::string kw_deinit = "deinit";
	inline const std::string kw_self = "self";

	// Integers
	inline const std::string kw_int8 = "int8";
	inline const std::string kw_int16 = "int16";
	inline const std::string kw_int32 = "int32";
	inline const std::string kw_int64 = "int64";
	inline const std::string kw_int = kw_int32;

	// Unsigned integers
	inline const std::string kw_uint8 = "uint8";
	inline const std::string kw_uint16 = "uint16";
	inline const std::string kw_uint32 = "uint32";
	inline const std::string kw_uint64 = "uint64";
	inline const std::string kw_uint = kw_uint32;

	// Floating point
	inline const std::string kw_float = "float";
	inline const std::string kw_double = "double";

	// Text
	inline const std::string kw_char = "char";
	inline const std::string kw_string = "string";

	// Bools
	inline const std::string kw_true = "true";
	inline const std::string kw_false = "false";
}

namespace dawn {
	// Math
	inline const std::string op_add = "+";
	inline const std::string op_sub = "-";
	inline const std::string op_mul = "*";
	inline const std::string op_div = "/";
	inline const std::string op_pow = "^";
	inline const std::string op_mod = "%";
	inline const std::string op_add_eq = "+=";
	inline const std::string op_sub_eq = "-=";
	inline const std::string op_mul_eq = "*=";
	inline const std::string op_div_eq = "/=";
	inline const std::string op_pow_eq = "%=";
	inline const std::string op_mod_eq = "^=";

	// Bools
	inline const std::string op_and = "&";
	inline const std::string op_or = "|";
	inline const std::string op_not = "!";
	inline const std::string op_less = "<";
	inline const std::string op_great = ">";
	inline const std::string op_eq = "==";
	inline const std::string op_not_eq = "!=";
	inline const std::string op_less_eq = "<=";
	inline const std::string op_great_eq = ">=";

	// Access
	inline const std::string op_ptr_access = "->";
}

namespace dawn {
	// Access
	inline constexpr char sep_assign = '=';
	inline constexpr char sep_split = ',';
	inline constexpr char sep_express = ';';
	inline constexpr char sep_access = '.';
	inline constexpr char sep_static_access = ':';

	// Control
	inline constexpr char sep_def_open = '(';
	inline constexpr char sep_def_close = ')';
	inline constexpr char sep_scope_open = '{';
	inline constexpr char sep_scope_close = '}';
	inline constexpr char sep_array_open = '[';
	inline constexpr char sep_array_close = ']';
}

namespace dawn {
	inline constexpr char misc_iden_ex = '_';
	inline constexpr char misc_dec_ex = '.';

	inline constexpr char misc_lit_char = '\'';
	inline constexpr char misc_lit_str = '"';

	inline const std::string misc_line_comm = "//";
	inline const std::pair<std::string, std::string> misc_multiline_comm = { "/*", "*/" };
}
