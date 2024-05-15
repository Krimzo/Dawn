#pragma once

#include "t.h"


namespace dawn {
	// Declarations
	class ModuleASTNode;
	class GlobalASTNode;
	class InternalASTNode;
	class InterfaceASTNode;
	class ClassASTNode;
	class StructASTNode;
	class EnumASTNode;
	class TypeASTNode;
	class DefASTNode;
	class LetASTNode;
	class VarASTNode;

	// Base
	class ASTNode
	{
	public:
		ASTNode() = default;
		virtual ~ASTNode() = default;
	};
}

namespace dawn {
	// Definitions
	class ModuleASTNode : public ASTNode
	{
	public:
		String name;
	};

	class GlobalASTNode : public ASTNode
	{
	public:
		Object<ModuleASTNode> module;
		Object<InternalASTNode> internal;
		Array<Object<InterfaceASTNode>> interfaces;
		Array<Object<ClassASTNode>> classes;
		Array<Object<StructASTNode>> structs;
		Array<Object<EnumASTNode>> enums;
		Array<Object<DefASTNode>> defs;
		Array<Object<LetASTNode>> lets;
		Array<Object<VarASTNode>> vars;
	};

	class InternalASTNode : public ASTNode
	{
	public:
		Array<Object<InterfaceASTNode>> interfaces;
		Array<Object<ClassASTNode>> classes;
		Array<Object<StructASTNode>> structs;
		Array<Object<EnumASTNode>> enums;
		Array<Object<DefASTNode>> defs;
		Array<Object<LetASTNode>> lets;
		Array<Object<VarASTNode>> vars;
	};

	class InterfaceASTNode : public ASTNode
	{
	public:
		String name;
		Array<Object<DefASTNode>> methods;
	};

	class ClassASTNode : public ASTNode
	{
	public:
		String name;
		Object<InternalASTNode> internal;
		Array<Object<InterfaceASTNode>> interfaces;
		Array<Object<VarASTNode>> members;
		Array<Object<DefASTNode>> constructors;
		Object<DefASTNode> destructor;
		Array<Object<DefASTNode>> methods;
	};

	class StructASTNode : public ASTNode
	{
	public:
		String name;
		Array<Object<VarASTNode>> members;
	};

	class EnumASTNode : public ASTNode
	{
	public:
		String name;
		Object<TypeASTNode> super_type;
		Array<Object<LetASTNode>> values;
	};

	class DefASTNode : public ASTNode
	{
	public:
		String name;
		Array<Object<LetASTNode>> args;
		Object<TypeASTNode> return_type;
		Object<ASTNode> body;
	};

	class LetASTNode : public ASTNode
	{
	public:
		String name;
		Object<TypeASTNode> type;
		Object<ASTNode> value;
	};

	class VarASTNode : public ASTNode
	{
	public:
		String name;
		Object<TypeASTNode> type;
		Object<ASTNode> value;
	};

	// Type
	enum PointerType
	{
		NONE = 0,
		VAR,
		LET,
	};

	class TypeASTNode : public ASTNode
	{
	public:
		String name;
		PointerType pointer_type;
	};
}

namespace dawn {
	class BoolASTNode;
	class IntASTNode;
	class UIntASTNode;
	class FloatASTNode;
	class CharASTNode;
	class StringASTNode;

	class OperatorASTNode : public ASTNode
	{
	public:
		OperatorASTNode() = default;

		
	};


}
