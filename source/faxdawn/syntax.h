#pragma once

#include "lexer/token.h"

#include <string>
#include <vector>
#include <unordered_set>


namespace faxdawn::syntax {
	namespace literal {
		inline const std::string comment   = "$";
		inline const std::string character = "'";
		inline const std::string string    = "\"";
		inline const std::string empty     = "none";
		inline const std::string true_     = "true";
		inline const std::string false_    = "false";
	}

	inline const std::unordered_set<std::string> ignored = {
		"\t", "\n",
	};

	inline const std::unordered_set<std::string> discarded = {
		"", " ",
	};
}

namespace faxdawn::syntax {
	namespace separator {
		inline const std::string space          = " ";
		inline const std::string end_command    = ";";
		inline const std::string elements_split = ",";
		inline const std::string open_scope     = "{";
		inline const std::string close_scope    = "}";
		inline const std::string open_function  = "(";
		inline const std::string close_function = ")";
		inline const std::string open_array     = "[";
		inline const std::string close_array    = "]";
	}
	
	inline const std::unordered_set separators = {
		separator::space,         
		separator::end_command,   
		separator::elements_split,
		separator::open_scope,    
		separator::close_scope,   
		separator::open_function, 
		separator::close_function,
		separator::open_array,    
		separator::close_array,   
	};
}

namespace faxdawn::syntax {
	namespace operator_ {
		inline const std::string assignment          = "=";
		inline const std::string access              = ":";
		inline const std::string nullable            = "?";
		inline const std::string final               = "!";
		inline const std::string add                 = "+";
		inline const std::string subtract            = "-";
		inline const std::string multiply            = "*";
		inline const std::string divide              = "/";
		inline const std::string power               = "^";
		inline const std::string modulo              = "%";
		inline const std::string add_self            = "+=";
		inline const std::string subtract_self       = "-=";
		inline const std::string multiply_self       = "*=";
		inline const std::string divide_self         = "/=";
		inline const std::string power_self          = "^=";
		inline const std::string modulo_self         = "%=";
		inline const std::string not_                = "!";
		inline const std::string and_                = "&";
		inline const std::string or_                 = "|";
		inline const std::string equals              = "==";
		inline const std::string not_equals          = "!=";
		inline const std::string less_than           = "<";
		inline const std::string greater_than        = ">";
		inline const std::string less_equals_than    = "<=";
		inline const std::string greater_equals_than = ">=";
	}

	inline const std::unordered_set operators = {
		operator_::assignment,         
		operator_::access,             
		operator_::nullable,           
		operator_::final,              
		operator_::add,                
		operator_::subtract,           
		operator_::multiply,           
		operator_::divide,             
		operator_::power,              
		operator_::modulo,             
		operator_::add_self,           
		operator_::subtract_self,      
		operator_::multiply_self,      
		operator_::divide_self,        
		operator_::power_self,         
		operator_::modulo_self,        
		operator_::not_,               
		operator_::and_,               
		operator_::or_,                
		operator_::equals,             
		operator_::not_equals,         
		operator_::less_than,          
		operator_::greater_than,       
		operator_::less_equals_than,   
		operator_::greater_equals_than,
	};
}

namespace faxdawn::syntax {
	namespace keyword {
		inline const std::string condition_init    = "if";
		inline const std::string condition_check   = "elif";
		inline const std::string condition_final   = "else";
		inline const std::string switch_init       = "switch";
		inline const std::string switch_check      = "case";
		inline const std::string switch_final      = "else";
		inline const std::string loop_init         = "loop";
		inline const std::string loop_stop         = "stop";
		inline const std::string loop_skip         = "skip";
		inline const std::string function_init     = "fu";
		inline const std::string function_evaluate = "eval";
		inline const std::string type_declaration  = "type";
		inline const std::string self_access       = "self";
		inline const std::string use_module        = "use";
	}
	
	inline const std::unordered_set keywords = {
		keyword::condition_init,
		keyword::condition_check,
		keyword::condition_final,
		keyword::switch_init,
		keyword::switch_check,
		keyword::switch_final,
		keyword::loop_init,
		keyword::loop_stop,
		keyword::loop_skip,
		keyword::function_init,
		keyword::function_evaluate,
		keyword::type_declaration,
		keyword::self_access,
		keyword::use_module,
	};
}

namespace faxdawn::syntax {
	inline const std::unordered_set<std::string> types = {
		"bool",
		"byte",
		"int",
		"float",
		"char",
		"string",
		"function",
		"thread",
	};
}

namespace faxdawn::syntax {
	token_type get_token_type(const std::unordered_set<std::string>& all_types, const std::string& value);
	
	bool is_discarded(const std::string& value);
	
	bool is_ignored(char value);
	bool is_ignored(const std::string& value);

	bool is_separator(char value);
	bool is_separator(const std::string& value);

	bool is_operator(char value);
	bool is_operator(const std::string& value);

	bool is_keyword(const std::string& value);
	
	bool is_literal(const std::string& value);
}
