#pragma once

#include "lexer.h"


namespace dawn {
	struct ParseError
	{
		String msg;

		template<typename... Args>
		ParseError(const Token& token, const Args&... args)
		{
			StringStream stream;
			stream << "Parse error at token " << token << ": ";
			(stream << ... << args);
			msg = stream.str();
		}

		inline operator const String& () const
		{
			return msg;
		}
	};

	std::wostream& operator<<(std::wostream& stream, const ParseError& error);
}

namespace dawn {
	struct Node;
}

namespace dawn {
	struct Type
	{
		String name;

		virtual ~Type() = default;
	};

	struct BoolType : Type
	{
		BoolType() { name = L"bool"; }
	};

	struct IntType : Type
	{
		IntType() { name = L"int"; }
	};

	struct FloatType : Type
	{
		FloatType() { name = L"float"; }
	};

	struct CharType : Type
	{
		CharType() { name = L"char"; }
	};

	struct StringType : Type
	{
		StringType() { name = L"string"; }
	};
}

namespace dawn {
	struct Value
	{
		virtual ~Value() = default;
	};

	struct PointerValue : Value
	{
		Pointer value = {};
	};

	struct BoolValue : Value
	{
		Bool value = {};
	};

	struct IntValue : Value
	{
		Int value = {};
	};

	struct FloatValue : Value
	{
		Float value = {};
	};

	struct CharValue : Value
	{
		Char value = {};
	};

	struct StringValue : Value
	{
		String value = {};
	};
}

namespace dawn {
	struct EnumType : Type
	{
		String value_type;
		Map<String, Any> values;
	};

	struct EnumValue : Value
	{
		String enum_type;
		String key;
	};
}

namespace dawn {
	struct MethodDecl
	{
		String name;
		bool is_var = false;
		Array<Pair<String, String>> args;
		String return_type;
	};

	struct Method
	{
		MethodDecl decl;
		Ref<Node> body;
	};

	struct LayerType : Type
	{
		Map<String, MethodDecl> methods;
	};

	struct StructType : Type
	{
		Map<String, String> fields_public;
		Map<String, Ref<Method>> methods_public;
		Map<String, String> fields_internal;
		Map<String, Ref<Method>> methods_internal;
	};

	struct StructValue : Value
	{
		String struct_type;
		Map<String, Any> members_public;
		Map<String, Any> members_internal;
	};
}

namespace dawn {
	struct Function
	{
		String name;
		Array<Pair<String, String>> args;
		String return_type;
		Ref<Node> body;
	};

	struct Variable
	{
		String name;
		bool is_var = false;
		String type;
		Ref<Node> value;
	};
}

namespace dawn {
	struct Global
	{
		Map<String, Variable> variables;
		Map<String, Function> functions;
		Map<String, EnumType> enums;
		Map<String, LayerType> layers;
		Map<String, StructType> structs;
	};
}

namespace dawn {
	struct Module
	{
		String name;
		Global global_public;
		Global global_internal;
	};
}

namespace dawn {
	struct Parser
	{
		Opt<ParseError> parse(const Array<Token>& tokens, Module& module);

	private:
		bool m_is_module_internal = false;
		bool m_is_struct_internal = false;

		Opt<ParseError> parse_module_module(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Module& module);
		Opt<ParseError> parse_module_internal(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Module& module);
		Opt<ParseError> parse_module_enum(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Module& module);
		Opt<ParseError> parse_module_layer(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Module& module);
		Opt<ParseError> parse_module_struct(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Module& module);
		Opt<ParseError> parse_module_function(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Module& module);
		Opt<ParseError> parse_module_variable(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Module& module);

		Opt<ParseError> parse_variable(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Variable& variable);
		Opt<ParseError> parse_type(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, String& type);
		Opt<ParseError> parse_expression(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Ref<Node>& tree);

		Opt<ParseError> extract_expression(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Array<Token>& tokens);
		Opt<ParseError> find_least_precedence(const Array<Token>& tokens, Int& index);
		Opt<ParseError> pure_expression(const Array<Token>& tokens, Ref<Node>& tree);
		Opt<ParseError> expression_literal(const Token& token, Ref<Node>& tree);
		Opt<ParseError> expression_identifier(const Token& token, Ref<Node>& tree);
		Opt<ParseError> expression_keyword(const Token& token, Ref<Node>& tree);
	};
}

namespace dawn {
	struct Node
	{
		virtual ~Node() = default;
	};
}

namespace dawn {
	struct ValueNode : Node
	{
		Ref<Value> value;
	};

	struct IdentifierNode : Node
	{
		String name;
	};

	struct SelfNode : Node
	{};
}

namespace dawn {
	struct UnaryNode : Node
	{
		Ref<Node> right;

		virtual Ref<Node> evaluate() const = 0;
	};

	struct UnaryNodePlus : UnaryNode
	{
		Ref<Node> evaluate() const override;
	};

	struct UnaryNodeMinus : UnaryNode
	{
		Ref<Node> evaluate() const override;
	};

	struct UnaryNodeNot : UnaryNode
	{
		Ref<Node> evaluate() const override;
	};

	struct UnaryNodeAddress : UnaryNode
	{
		Ref<Node> evaluate() const override;
	};

	Opt<ParseError> create_unary_node(const Token& token, Ref<UnaryNode>& node);
}

namespace dawn {
	struct OperatorNode : Node
	{
		Ref<Node> left;
		Ref<Node> right;

		virtual Ref<Node> evaluate() = 0;
	};

	struct OperatorNodeLink : OperatorNode
	{
		Ref<Node> evaluate() override;
	};

	struct OperatorNodeAccess : OperatorNode
	{
		Ref<Node> evaluate() override;
	};

	struct OperatorNodeAddress : OperatorNode
	{
		Ref<Node> evaluate() override;
	};

	struct OperatorNodeNot : OperatorNode
	{
		Ref<Node> evaluate() override;
	};

	struct OperatorNodePow : OperatorNode
	{
		Ref<Node> evaluate() override;
	};

	struct OperatorNodeMod : OperatorNode
	{
		Ref<Node> evaluate() override;
	};

	struct OperatorNodeMul : OperatorNode
	{
		Ref<Node> evaluate() override;
	};

	struct OperatorNodeDiv : OperatorNode
	{
		Ref<Node> evaluate() override;
	};

	struct OperatorNodeAdd : OperatorNode
	{
		Ref<Node> evaluate() override;
	};

	struct OperatorNodeSub : OperatorNode
	{
		Ref<Node> evaluate() override;
	};

	struct OperatorNodeLess : OperatorNode
	{
		Ref<Node> evaluate() override;
	};

	struct OperatorNodeGreat : OperatorNode
	{
		Ref<Node> evaluate() override;
	};

	struct OperatorNodeLessEq : OperatorNode
	{
		Ref<Node> evaluate() override;
	};

	struct OperatorNodeGreatEq : OperatorNode
	{
		Ref<Node> evaluate() override;
	};

	struct OperatorNodeEq : OperatorNode
	{
		Ref<Node> evaluate() override;
	};

	struct OperatorNodeNotEq : OperatorNode
	{
		Ref<Node> evaluate() override;
	};

	struct OperatorNodeAnd : OperatorNode
	{
		Ref<Node> evaluate() override;
	};

	struct OperatorNodeOr : OperatorNode
	{
		Ref<Node> evaluate() override;
	};

	struct OperatorNodeAssign : OperatorNode
	{
		Ref<Node> evaluate() override;
	};

	struct OperatorNodeAddAs : OperatorNode
	{
		Ref<Node> evaluate() override;
	};

	struct OperatorNodeSubAs : OperatorNode
	{
		Ref<Node> evaluate() override;
	};

	struct OperatorNodeMulAs : OperatorNode
	{
		Ref<Node> evaluate() override;
	};

	struct OperatorNodeDivAs : OperatorNode
	{
		Ref<Node> evaluate() override;
	};

	struct OperatorNodePowAs : OperatorNode
	{
		Ref<Node> evaluate() override;
	};

	struct OperatorNodeModAs : OperatorNode
	{
		Ref<Node> evaluate() override;
	};

	struct OperatorNodeSplit : OperatorNode
	{
		Ref<Node> evaluate() override;
	};

	Opt<ParseError> create_operator_node(const Token& token, Ref<OperatorNode>& node);
}
