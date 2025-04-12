#pragma once

#include "util.h"


namespace dawn
{
inline constexpr StringRef kw_import = "import";
inline constexpr StringRef kw_func = "func";
inline constexpr StringRef kw_oper = "oper";
inline constexpr StringRef kw_return = "return";
inline constexpr StringRef kw_var = "var";
inline constexpr StringRef kw_ref = "ref";
inline constexpr StringRef kw_if = "if";
inline constexpr StringRef kw_else = "else";
inline constexpr StringRef kw_elif = "elif";
inline constexpr StringRef kw_switch = "switch";
inline constexpr StringRef kw_case = "case";
inline constexpr StringRef kw_default = "default";
inline constexpr StringRef kw_for = "for";
inline constexpr StringRef kw_while = "while";
inline constexpr StringRef kw_loop = "loop";
inline constexpr StringRef kw_break = "break";
inline constexpr StringRef kw_continue = "continue";
inline constexpr StringRef kw_throw = "throw";
inline constexpr StringRef kw_try = "try";
inline constexpr StringRef kw_catch = "catch";
inline constexpr StringRef kw_enum = "enum";
inline constexpr StringRef kw_struct = "struct";
inline constexpr StringRef kw_self = "self";
inline constexpr StringRef kw_true = "true";
inline constexpr StringRef kw_false = "false";

inline constexpr StringRef tp_nothing = "nothing";
inline constexpr StringRef tp_bool = "bool";
inline constexpr StringRef tp_int = "int";
inline constexpr StringRef tp_float = "float";
inline constexpr StringRef tp_char = "char";
inline constexpr StringRef tp_string = "string";
inline constexpr StringRef tp_function = "function";
inline constexpr StringRef tp_enum = "enum";
inline constexpr StringRef tp_struct = "struct";
inline constexpr StringRef tp_array = "array";
inline constexpr StringRef tp_range = "range";

inline constexpr StringRef op_add = "+";
inline constexpr StringRef op_sub = "-";
inline constexpr StringRef op_mul = "*";
inline constexpr StringRef op_div = "/";
inline constexpr StringRef op_pow = "^";
inline constexpr StringRef op_mod = "%";
inline constexpr StringRef op_addas = "+=";
inline constexpr StringRef op_subas = "-=";
inline constexpr StringRef op_mulas = "*=";
inline constexpr StringRef op_divas = "/=";
inline constexpr StringRef op_powas = "^=";
inline constexpr StringRef op_modas = "%=";
inline constexpr StringRef op_not = "!";
inline constexpr StringRef op_and = "&&";
inline constexpr StringRef op_or = "||";
inline constexpr StringRef op_cmpr = "<=>";
inline constexpr StringRef op_eq = "==";
inline constexpr StringRef op_neq = "!=";
inline constexpr StringRef op_less = "<";
inline constexpr StringRef op_great = ">";
inline constexpr StringRef op_lesseq = "<=";
inline constexpr StringRef op_greateq = ">=";
inline constexpr StringRef op_assign = "=";
inline constexpr StringRef op_access = "->";
inline constexpr StringRef op_range = ">>";

inline constexpr StringRef op_link = ":";
inline constexpr StringRef op_split = ",";
inline constexpr StringRef op_lambda = "`";
inline constexpr StringRef op_expr_opn = "(";
inline constexpr StringRef op_expr_cls = ")";
inline constexpr StringRef op_scope_opn = "{";
inline constexpr StringRef op_scope_cls = "}";
inline constexpr StringRef op_array_opn = "[";
inline constexpr StringRef op_array_cls = "]";

inline constexpr StringRef sep_number = ".";
inline constexpr StringRef sep_identifier = "_";
inline constexpr StringRef lit_char = "'";
inline constexpr StringRef lit_string = "\"";
inline constexpr StringRef comm_line = "//";
inline constexpr Pair<StringRef, StringRef> comm_multiline = { "/*", "*/" };

inline const String prec_unary = "_unary";

inline HMap<String, Int> precedences
{
    { (String) op_access, 1 },
    { (String) op_expr_opn, 1 },
    { (String) op_scope_opn, 1 },
    { (String) op_array_opn, 1 },

    { (String) prec_unary, 2 },
    { (String) op_not, 2 },

    { (String) op_pow, 3 },
    { (String) op_mod, 3 },

    { (String) op_mul, 4 },
    { (String) op_div, 4 },

    { (String) op_add, 5 },
    { (String) op_sub, 5 },

    { (String) op_cmpr, 6 },

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
};
}
