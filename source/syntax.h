#pragma once

#include "util.h"

namespace dawn
{
inline constexpr StringRef kw_import = "import";
inline constexpr StringRef kw_func = "func";
inline constexpr StringRef kw_cast = "cast";
inline constexpr StringRef kw_oper = "oper";
inline constexpr StringRef kw_return = "return";
inline constexpr StringRef kw_if = "if";
inline constexpr StringRef kw_elif = "elif";
inline constexpr StringRef kw_else = "else";
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

inline constexpr StringRef tp_void = "void";
inline constexpr StringRef tp_bool = "bool";
inline constexpr StringRef tp_int = "int";
inline constexpr StringRef tp_float = "float";
inline constexpr StringRef tp_char = "char";
inline constexpr StringRef tp_string = "string";
inline constexpr StringRef tp_range = "range";
inline constexpr StringRef tp_function = "function";
inline constexpr StringRef tp_array = "array";

inline constexpr StringRef vr_constant = "";
inline constexpr StringRef vr_variable = "?";
inline constexpr StringRef vr_reference = "&";

inline constexpr StringRef op_add = "+";
inline constexpr StringRef op_sub = "-";
inline constexpr StringRef op_mul = "*";
inline constexpr StringRef op_div = "/";
inline constexpr StringRef op_pow = "^";
inline constexpr StringRef op_mod = "%";
inline constexpr StringRef op_eq = "==";
inline constexpr StringRef op_neq = "!=";
inline constexpr StringRef op_less = "<";
inline constexpr StringRef op_great = ">";
inline constexpr StringRef op_lesseq = "<=";
inline constexpr StringRef op_greateq = ">=";
inline constexpr StringRef op_not = "!";
inline constexpr StringRef op_and = "and";
inline constexpr StringRef op_or = "or";
inline constexpr StringRef op_range = ">>";
inline constexpr StringRef op_range_incl = "=>";
inline constexpr StringRef op_cast = "@";

inline constexpr StringRef op_assign = "=";
inline constexpr StringRef op_addas = "+=";
inline constexpr StringRef op_subas = "-=";
inline constexpr StringRef op_mulas = "*=";
inline constexpr StringRef op_divas = "/=";
inline constexpr StringRef op_powas = "^=";
inline constexpr StringRef op_modas = "%=";

inline constexpr StringRef op_link = "::";
inline constexpr StringRef op_point = "->";
inline constexpr StringRef op_access = ".";
inline constexpr StringRef op_set = ":";
inline constexpr StringRef op_split = ",";
inline constexpr StringRef op_lambda = "`";
inline constexpr StringRef op_expr_opn = "(";
inline constexpr StringRef op_expr_cls = ")";
inline constexpr StringRef op_scope_opn = "{";
inline constexpr StringRef op_scope_cls = "}";
inline constexpr StringRef op_array_opn = "[";
inline constexpr StringRef op_array_cls = "]";

inline constexpr StringRef exp_number = "e";
inline constexpr StringRef exp_end = ";";
inline constexpr StringRef sep_number = ".";
inline constexpr StringRef sep_identifier = "_";
inline constexpr StringRef lit_char = "'";
inline constexpr StringRef lit_string = "\"";
inline constexpr StringRef comm_line = "//";
inline constexpr Pair<StringRef, StringRef> comm_multiline = {"/*", "*/"};

inline constexpr StringRef prec_unary = "_unary";

Int _get_prec();
Int _incr_get_prec();

inline const StringMap<Int> precedences{
    {(String)op_link, _incr_get_prec()},

    {(String)op_access, _incr_get_prec()},  {(String)op_expr_opn, _get_prec()},   {(String)op_scope_opn, _get_prec()},
    {(String)op_array_opn, _get_prec()},

    {(String)op_point, _incr_get_prec()},

    {(String)prec_unary, _incr_get_prec()}, {(String)op_not, _get_prec()},

    {(String)op_cast, _incr_get_prec()},

    {(String)op_pow, _incr_get_prec()},     {(String)op_mod, _get_prec()},

    {(String)op_mul, _incr_get_prec()},     {(String)op_div, _get_prec()},

    {(String)op_add, _incr_get_prec()},     {(String)op_sub, _get_prec()},

    {(String)op_less, _incr_get_prec()},    {(String)op_great, _get_prec()},      {(String)op_lesseq, _get_prec()},
    {(String)op_greateq, _get_prec()},

    {(String)op_eq, _incr_get_prec()},      {(String)op_neq, _get_prec()},

    {(String)op_and, _incr_get_prec()},

    {(String)op_or, _incr_get_prec()},

    {(String)op_range, _incr_get_prec()},   {(String)op_range_incl, _get_prec()},

    {(String)op_assign, _incr_get_prec()},  {(String)op_addas, _get_prec()},      {(String)op_subas, _get_prec()},
    {(String)op_mulas, _get_prec()},        {(String)op_divas, _get_prec()},      {(String)op_powas, _get_prec()},
    {(String)op_modas, _get_prec()},
};
} // namespace dawn
