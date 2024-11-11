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
            stream << "Parse error at token " << *token << ": ";
        }
        else
        {
            stream << "Parse error: ";
        }
        (stream << ... << args);
        msg = stream.str();
    }

    inline operator String const& () const
    {
        return msg;
    }
};

std::wostream& operator<<( std::wostream& stream, ParseError const& error );
}

namespace dawn
{
struct Node;
}

namespace dawn
{
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
}

namespace dawn
{
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

namespace dawn
{
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

namespace dawn
{
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

namespace dawn
{
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

namespace dawn
{
struct Space
{
    Map<String, Variable> variables;
    Map<String, Function> functions;
    Map<String, EnumType> enums;
    Map<String, LayerType> layers;
    Map<String, StructType> structs;

    Bool contains_id( StringRef const& id ) const;
};
}

namespace dawn
{
struct Module
{
    String name;
    Space space_public;
    Space space_internal;

    Bool contains_id( StringRef const& id ) const;
};
}

namespace dawn
{
struct Parser
{
    Opt<ParseError> parse( Array<Token>& tokens, Module& module );

private:
    Bool m_is_module_internal = false;
    Bool m_is_struct_internal = false;

    void prepare_tokens( Array<Token>& tokens );

    Opt<ParseError> parse_module_module( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module );
    Opt<ParseError> parse_module_internal( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module );
    Opt<ParseError> parse_module_enum( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module );
    Opt<ParseError> parse_module_layer( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module );
    Opt<ParseError> parse_module_struct( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module );
    Opt<ParseError> parse_module_function( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module );
    Opt<ParseError> parse_module_variable( Array<Token>::const_iterator& it, Array<Token>::const_iterator const& end, Module& module );

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
};
}

namespace dawn
{
struct Node
{
    virtual ~Node() = default;
};
}

namespace dawn
{
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
}

namespace dawn
{
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
}

namespace dawn
{
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

struct OperatorNodeSplit : OperatorNode
{
};

Opt<ParseError> create_operator_node( Token const& token, Ref<OperatorNode>& node );
}
