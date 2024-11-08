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

	struct RefType : Type
	{
		Ref<Type> type;
	};

	struct LetRefType : RefType
	{
		LetRefType() { name = String(kw_let) + String(op_ref); }
	};

	struct VarRefType : RefType
	{
		VarRefType() { name = String(kw_var) + String(op_ref); }
	};

	struct ArrayType : Type
	{
		Ref<Type> type;

		ArrayType() { name = String(op_array_opn) + String(op_array_cls); }
	};

	struct RangeType : Type
	{
		RangeType() { name = op_range; }
	};

	struct BoolType : Type
	{
		BoolType() { name = tp_bool; }
	};

	struct IntType : Type
	{
		IntType() { name = tp_int; }
	};

	struct FloatType : Type
	{
		FloatType() { name = tp_float; }
	};

	struct CharType : Type
	{
		CharType() { name = tp_char; }
	};

	struct StringType : Type
	{
		StringType() { name = tp_string; }
	};
}

namespace dawn {
	struct Value
	{
		virtual ~Value() = default;
	};

	struct ReferenceValue : Value
	{
		Ref<Value> value = {};
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
		Ref<Type> type;
		Map<String, Ref<Value>> values;
	};

	struct EnumValue : Value
	{
		Ref<EnumType> parent;
		String key;
	};
}

namespace dawn {
	struct MethodDecl
	{
		String name;
		Bool is_var = false;
		Array<Pair<String, Ref<Type>>> args;
		Ref<Type> type;
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
		Map<String, Ref<Type>> fields_public;
		Map<String, Ref<Method>> methods_public;
		Map<String, Ref<Type>> fields_internal;
		Map<String, Ref<Method>> methods_internal;
	};

	struct StructValue : Value
	{
		Ref<StructType> parent;
		Map<String, Ref<Value>> members_public;
		Map<String, Ref<Value>> members_internal;
	};
}

namespace dawn {
	struct Function
	{
		String name;
		Array<Pair<String, Ref<Type>>> args;
		Ref<Type> type;
		Ref<Node> body;
	};

	struct Variable
	{
		String name;
		Bool is_var = false;
		Ref<Type> type;
		Ref<Node> value;
	};
}

namespace dawn {
	struct Space
	{
		Map<String, Variable> variables;
		Map<String, Function> functions;
		Map<String, EnumType> enums;
		Map<String, LayerType> layers;
		Map<String, StructType> structs;

		Bool contains_id(const StringRef& id) const;
	};
}

namespace dawn {
	struct Module
	{
		String name;
		Space space_public;
		Space space_internal;

		Bool contains_id(const StringRef& id) const;
	};
}

namespace dawn {
	struct Parser
	{
		Opt<ParseError> parse(Array<Token>& tokens, Module& module);

	private:
		Bool m_is_module_internal = false;
		Bool m_is_struct_internal = false;

		void prepare_tokens(Array<Token>& tokens);

		Opt<ParseError> parse_module_module(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Module& module);
		Opt<ParseError> parse_module_internal(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Module& module);
		Opt<ParseError> parse_module_enum(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Module& module);
		Opt<ParseError> parse_module_layer(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Module& module);
		Opt<ParseError> parse_module_struct(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Module& module);
		Opt<ParseError> parse_module_function(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Module& module);
		Opt<ParseError> parse_module_variable(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Module& module);

		Opt<ParseError> parse_variable(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Variable& variable);
		Opt<ParseError> parse_type(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Ref<Type>& type);
		Opt<ParseError> parse_reference_type(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Ref<RefType>& type);
		Opt<ParseError> parse_expression(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Ref<Node>& tree);

		Opt<ParseError> expression_extract(Array<Token>::const_iterator& it, const Array<Token>::const_iterator& end, Array<Token>& tokens);
		Opt<ParseError> expression_precedence(const Array<Token>& tokens, Int& index);
		Opt<ParseError> expression_pure(const Array<Token>& tokens, Ref<Node>& tree);

		Opt<ParseError> expression_single(const Token& token, Ref<Node>& tree);
		Opt<ParseError> expression_single_literal(const Token& token, Ref<Node>& tree);
		Opt<ParseError> expression_single_keyword(const Token& token, Ref<Node>& tree);
		Opt<ParseError> expression_single_identifier(const Token& token, Ref<Node>& tree);
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

	struct UnaryNodeRef : UnaryNode
	{
		Ref<Node> evaluate() const override;
	};

	struct UnaryNodeRange : UnaryNode
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

	struct OperatorNodeRange : OperatorNode
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
