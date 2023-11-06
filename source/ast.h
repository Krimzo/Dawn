#pragma once

#include "cpp_helper.h"


namespace dawn {
	// Declarations
	struct GlobalASTNode;
	struct ScopeASTNode;
	struct ModuleASTNode;
	struct InternalASTNode;
	struct LetASTNode;
	struct VarASTNode;
	struct DefASTNode;
	struct EnumASTNode;
	struct StructASTNode;
	struct InterfaceASTNode;
	struct ClassASTNode;

	// Base
	struct ASTNode
	{
		ASTNode() = default;
		virtual ~ASTNode() = default;
	};

	// Definitions
	struct GlobalASTNode : public ASTNode
	{
		std::shared_ptr<ModuleASTNode> module;
		std::vector<std::shared_ptr<InternalASTNode>> internals;
		std::vector<std::shared_ptr<InterfaceASTNode>> interfaces;
		std::vector<std::shared_ptr<ClassASTNode>> classes;
		std::vector<std::shared_ptr<StructASTNode>> structs;
		std::vector<std::shared_ptr<EnumASTNode>> enums;
		std::vector<std::shared_ptr<DefASTNode>> defs;
		std::vector<std::shared_ptr<VarASTNode>> vars;
		std::vector<std::shared_ptr<LetASTNode>> lets;
	};

	struct ScopeASTNode : public ASTNode
	{
		std::vector<std::shared_ptr<ASTNode>> instructions;
	};

	struct ModuleASTNode : public ASTNode
	{
		std::string module_name;
	};

	struct InternalASTNode : public ASTNode
	{
		std::vector<std::shared_ptr<InterfaceASTNode>> interfaces;
		std::vector<std::shared_ptr<ClassASTNode>> classes;
		std::vector<std::shared_ptr<StructASTNode>> structs;
		std::vector<std::shared_ptr<EnumASTNode>> enums;
		std::vector<std::shared_ptr<DefASTNode>> defs;
		std::vector<std::shared_ptr<VarASTNode>> vars;
		std::vector<std::shared_ptr<LetASTNode>> lets;
	};

	struct LetASTNode : public ASTNode
	{
		std::string name;
		std::string type;
		std::shared_ptr<ASTNode> value;
	};

	struct VarASTNode : public ASTNode
	{
		std::string name;
		std::string type;
		std::shared_ptr<ASTNode> value;
	};

	struct DefASTNode : public ASTNode
	{
		std::string name;
		std::unordered_map<std::string, std::pair<std::string, std::shared_ptr<ASTNode>>> args;
		std::string return_type;
		std::shared_ptr<ScopeASTNode> scope;
	};

	struct EnumASTNode : public ASTNode
	{
		std::string name;
		std::string type;
		std::unordered_map<std::string, std::shared_ptr<ASTNode>> values;
	};

	struct StructASTNode : public ASTNode
	{
		std::string name;
		std::unordered_map<std::string, std::pair<std::string, std::shared_ptr<ASTNode>>> members;
	};

	struct InterfaceASTNode : public ASTNode
	{
		std::string name;
		std::vector<std::shared_ptr<DefASTNode>> methods;
	};

	struct ClassASTNode : public ASTNode
	{
		std::string name;
		
		// Interfaces
		std::unordered_set<std::string> interfaces;
		std::vector<std::shared_ptr<DefASTNode>> implemented_methods;

		// Members
		std::unordered_map<std::string, std::pair<std::string, std::shared_ptr<ASTNode>>> members;
		std::unordered_map<std::string, std::pair<std::string, std::shared_ptr<ASTNode>>> internal_members;

		// Init/deinit
		std::vector<std::shared_ptr<DefASTNode>> constructors;
		std::shared_ptr<ScopeASTNode> destructor;

		// Methods
		std::vector<std::shared_ptr<DefASTNode>> methods;
		std::vector<std::shared_ptr<DefASTNode>> internal_methods;
	};
}
