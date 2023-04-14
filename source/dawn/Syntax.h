#pragma once

#include "lexer/Token.h"


namespace dawn::synatx {
	inline const Set<String> ignored = {
		L"\t", L"\n",
	};

	inline const Set<String> discarded = {
		L"", L" ",
	};
}

namespace dawn::literal {
	inline const String   comment = L"$";
	inline const String character = L"'";
	inline const String    string = L"\"";
	inline const String     empty = L"none";
	inline const String     true_ = L"true";
	inline const String    false_ = L"false";
}

namespace dawn::separator {
	inline const String          space = L" ";
	inline const String    end_command = L";";
	inline const String elements_split = L",";
	inline const String     open_scope = L"{";
	inline const String    close_scope = L"}";
	inline const String  open_function = L"(";
	inline const String close_function = L")";
	inline const String     open_array = L"[";
	inline const String    close_array = L"]";
	
	inline const Set<String> all = {
		space,         
		end_command,   
		elements_split,
		open_scope,    
		close_scope,   
		open_function, 
		close_function,
		open_array,    
		close_array,   
	};
}

namespace dawn::operator_ {
	inline const String          assignment = L"=";
	inline const String              access = L":";
	inline const String            nullable = L"?";
	inline const String                 add = L"+";
	inline const String            subtract = L"-";
	inline const String            multiply = L"*";
	inline const String              divide = L"/";
	inline const String               power = L"^";
	inline const String              modulo = L"%";
	inline const String            add_self = L"+=";
	inline const String       subtract_self = L"-=";
	inline const String       multiply_self = L"*=";
	inline const String         divide_self = L"/=";
	inline const String          power_self = L"^=";
	inline const String         modulo_self = L"%=";
	inline const String                not_ = L"!";
	inline const String                and_ = L"&";
	inline const String                 or_ = L"|";
	inline const String              equals = L"==";
	inline const String          not_equals = L"!=";
	inline const String           less_than = L"<";
	inline const String        greater_than = L">";
	inline const String    less_equals_than = L"<=";
	inline const String greater_equals_than = L">=";

	inline const Set<String> all = {
		assignment,         
		access,             
		nullable,            
		add,                
		subtract,           
		multiply,           
		divide,             
		power,              
		modulo,             
		add_self,           
		subtract_self,      
		multiply_self,      
		divide_self,        
		power_self,         
		modulo_self,        
		not_,               
		and_,               
		or_,                
		equals,             
		not_equals,         
		less_than,          
		greater_than,       
		less_equals_than,   
		greater_equals_than,
	};
}

namespace dawn::keyword {
	inline const String    condition_init = L"if";
	inline const String   condition_check = L"elif";
	inline const String   condition_final = L"else";
	inline const String       switch_init = L"switch";
	inline const String      switch_check = L"case";
	inline const String      switch_final = L"else";
	inline const String         loop_init = L"loop";
	inline const String         loop_stop = L"stop";
	inline const String         loop_skip = L"skip";
	inline const String     function_init = L"fu";
	inline const String function_evaluate = L"eval";
	inline const String  type_declaration = L"type";
	inline const String       self_access = L"self";
	inline const String        use_module = L"use";
	
	inline const Set<String> all = {
		condition_init,
		condition_check,
		condition_final,
		switch_init,
		switch_check,
		switch_final,
		loop_init,
		loop_stop,
		loop_skip,
		function_init,
		function_evaluate,
		type_declaration,
		self_access,
		use_module,
	};
}

namespace dawn::types {
	inline const Set<String> all = {
		L"Bool",
		L"Byte",
		L"Int",
		L"Float",
		L"Char",
		L"String",
	};
}

namespace dawn {
	TokenType GetTokenType(const Set<String>& allTypes, const String& value);

	bool IsDiscarded(const String& value);
	bool IsIgnored(const String& value);
	bool IsSeparator(const String& value);
	bool IsOperator(const String& value);
	bool IsKeyword(const String& value);
	bool IsLiteral(const String& value);

	bool IsIgnored(Char value);
	bool IsSeparator(Char value);
	bool IsOperator(Char value);

	bool IsFloat(const String& value);
	bool IsInt(const String& value);
}
