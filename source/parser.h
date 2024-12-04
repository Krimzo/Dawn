#pragma once

#include "lexer.h"


namespace dawn
{
struct ParseError
{
    String msg;

    template<typename... Args>
    ParseError( Opt<Token> const& token, Args const&... args )
    {
        StringStream stream;
        if ( token )
        {
            stream << L"Parse error at token " << *token << L": ";
        }
        else
        {
            stream << L"Parse error: ";
        }
        (stream << ... << args);
        msg = stream.str();
    }

    inline operator auto& () const
    {
        return msg;
    }
};

std::wostream& operator<<( std::wostream& stream, ParseError const& error );

struct Node
{
    virtual ~Node() = default;
};

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
    LetRefType() { name = String( kw_let ) + String( op_ref ); }
};

struct VarRefType : RefType
{
    VarRefType() { name = String( kw_var ) + String( op_ref ); }
};

struct ArrayType : Type
{
    Ref<Type> type;

    ArrayType() { name = String( op_array_opn ) + String( op_array_cls ); }
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

struct Value
{
    virtual ~Value() = default;

    virtual String to_string() const = 0;
};

struct ReferenceValue : Value
{
    Ref<Value> value = {};

    String to_string() const override;
};

struct ArrayValue : Value, Makeable<ArrayValue>
{
    Array<Ref<Value>> data = {};

    String to_string() const override;
};

struct BoolValue : Value, Makeable<BoolValue>
{
    Bool value = {};

    String to_string() const override;
};

struct IntValue : Value, Makeable<IntValue>
{
    Int value = {};

    String to_string() const override;
};

struct FloatValue : Value, Makeable<FloatValue>
{
    Float value = {};

    String to_string() const override;
};

struct CharValue : Value, Makeable<CharValue>
{
    Char value = {};

    String to_string() const override;
};

struct StringValue : Value, Makeable<StringValue>
{
    String value = {};

    String to_string() const override;
};

struct Variable
{
    String name;
    Bool is_var = false;
    Ref<Type> type;
    Ref<Node> expr;
};

struct Scope : Node
{
    Array<Ref<Node>> instr;
};

struct Argument
{
    String name;
    Ref<Type> type;
};

struct Function
{
    String name;
    Array<Argument> args;
    Ref<Type> type;
    Scope body;
};

struct Operator
{
    String name;
    Array<Argument> args;
    Ref<Type> type;
    Scope body;
};

struct MethodDecl
{
    String name;
    Bool is_var = false;
    Array<Argument> args;
    Ref<Type> type;
};

struct Method
{
    MethodDecl decl;
    Scope body;
};

struct EnumType : Type
{
    Ref<Type> type;
    Map<String, Ref<Value>> values;
};

struct EnumValue : Value, Makeable<EnumValue>
{
    Ref<EnumType> parent;
    String key;

    String to_string() const override;
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

struct StructValue : Value, Makeable<StructValue>
{
    Ref<StructType> parent;
    Map<String, Ref<Value>> members_public;
    Map<String, Ref<Value>> members_internal;

    String to_string() const override;
};

struct Module
{
    String name;
    Map<String, Variable> variables;
    Map<String, Operator> operators;
    Map<String, Function> functions;
    Map<String, EnumType> enums;
    Map<String, LayerType> layers;
    Map<String, StructType> structs;

    Bool contains_id( StringRef const& id ) const;
};

struct Parser
{
    Opt<ParseError> parse( Array<Token>& tokens, Module& module );

private:
    void prepare_tokens( Array<Token>& tokens );

    Opt<ParseError> parse_module_module( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module );
    Opt<ParseError> parse_module_struct( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module );
    Opt<ParseError> parse_module_layer( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module );
    Opt<ParseError> parse_module_enum( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module );
    Opt<ParseError> parse_module_function( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module );
    Opt<ParseError> parse_module_operator( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module );
    Opt<ParseError> parse_module_variable( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module );

    Opt<ParseError> parse_struct( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, StructType& struct_type );
    Opt<ParseError> parse_layer( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, LayerType& layer_type );
    Opt<ParseError> parse_enum( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, EnumType& enum_type );
    Opt<ParseError> parse_function( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Function& function );
    Opt<ParseError> parse_operator( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Operator& operato );
    Opt<ParseError> parse_variable( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Variable& variable );

    Opt<ParseError> parse_type( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Type>& type );
    Opt<ParseError> type_basic( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Type>& type );
    Opt<ParseError> type_reference( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<RefType>& type );

    Opt<ParseError> parse_expression( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Node>& tree );
    Opt<ParseError> expression_extract( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Array<Token>& tokens );
    Opt<ParseError> expression_precedence( Array<Token> const& tokens, Int& index );
    Opt<ParseError> expression_pure( Array<Token> const& tokens, Ref<Node>& tree );
    Opt<ParseError> expression_single( Token const& token, Ref<Node>& tree );
    Opt<ParseError> expression_single_literal( Token const& token, Ref<Node>& tree );
    Opt<ParseError> expression_single_keyword( Token const& token, Ref<Node>& tree );
    Opt<ParseError> expression_single_identifier( Token const& token, Ref<Node>& tree );
    Opt<ParseError> expression_type( Array<Token> const& tokens, Ref<Node>& tree );
    Opt<ParseError> expression_type_cast( Array<Token> const& tokens, Ref<Node>& tree );
    Opt<ParseError> expression_type_make( Array<Token> const& tokens, Ref<Node>& tree );
    Opt<ParseError> expression_type_array( Array<Token> const& tokens, Ref<Node>& tree );
    Opt<ParseError> expression_function( Array<Token> const& tokens, Ref<Node>& tree );
    Opt<ParseError> expression_yield( Array<Token> const& tokens, Ref<Node>& tree );

    Opt<ParseError> parse_scope( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Scope& scope );
    Opt<ParseError> scope_if( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Node>& tree );
    Opt<ParseError> scope_switch( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Node>& tree );
    Opt<ParseError> scope_for( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Node>& tree );
    Opt<ParseError> scope_while( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Node>& tree );
    Opt<ParseError> scope_loop( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Node>& tree );
    Opt<ParseError> scope_return( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Ref<Node>& tree );
};

struct ValueNode : Node
{
    Ref<Value> value;
};

struct SelfNode : Node
{
};

struct IdentifierNode : Node
{
    String name;
};

struct CastNode : Node
{
    Ref<Type> type;
    Ref<Node> expr;
};

struct NewStructNode : Node
{
    Ref<Type> type;
    Map<String, Ref<Node>> args;
};

struct NewArrayNode : Node
{
    enum struct InitType
    {
        SIZE,
        SIZE_VAL,
        LIST,
    };

    Ref<Type> type;
    InitType init_type;
    Ref<Node> size;
    Ref<Node> _val;
    Array<Ref<Node>> _list;
};

struct NewVarNode : Node
{
    Variable var;
};

struct FunctionCallNode : Node
{
    String name;
    Array<Ref<Node>> args;
};

struct ReturnNode : Node
{
    Ref<Node> expr;
};

struct PrintNode : Node
{
    Ref<Node> expr;
};

struct UnaryNode : Node
{
    Ref<Node> right;
};

struct UnaryNodePlus : UnaryNode
{
};

struct UnaryNodeMinus : UnaryNode
{
};

struct UnaryNodeNot : UnaryNode
{
};

struct UnaryNodeRef : UnaryNode
{
};

struct UnaryNodeRange : UnaryNode
{
};

Opt<ParseError> create_unary_node( Token const& token, Ref<UnaryNode>& node );

struct OperatorNode : Node
{
    Ref<Node> left;
    Ref<Node> right;
};

struct OperatorNodeLink : OperatorNode
{
};

struct OperatorNodeAccess : OperatorNode
{
};

struct OperatorNodeRange : OperatorNode
{
};

struct OperatorNodePow : OperatorNode
{
};

struct OperatorNodeMod : OperatorNode
{
};

struct OperatorNodeMul : OperatorNode
{
};

struct OperatorNodeDiv : OperatorNode
{
};

struct OperatorNodeAdd : OperatorNode
{
};

struct OperatorNodeSub : OperatorNode
{
};

struct OperatorNodeLess : OperatorNode
{
};

struct OperatorNodeGreat : OperatorNode
{
};

struct OperatorNodeLessEq : OperatorNode
{
};

struct OperatorNodeGreatEq : OperatorNode
{
};

struct OperatorNodeEq : OperatorNode
{
};

struct OperatorNodeNotEq : OperatorNode
{
};

struct OperatorNodeAnd : OperatorNode
{
};

struct OperatorNodeOr : OperatorNode
{
};

struct OperatorNodeAssign : OperatorNode
{
};

struct OperatorNodeAddAs : OperatorNode
{
};

struct OperatorNodeSubAs : OperatorNode
{
};

struct OperatorNodeMulAs : OperatorNode
{
};

struct OperatorNodeDivAs : OperatorNode
{
};

struct OperatorNodePowAs : OperatorNode
{
};

struct OperatorNodeModAs : OperatorNode
{
};

Opt<ParseError> create_operator_node( Token const& token, Ref<OperatorNode>& node );
}
