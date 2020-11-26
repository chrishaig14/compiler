//
// Created by chris on 1/8/20.
//

#ifndef NODE_H
#define NODE_H

#include "json.hpp"
#include "node_types.h"
#include <string>

using json = nlohmann::json;

class AssignmentNode;

class BinopNode;

class BlockNode;

class BooleanNode;

class BreakNode;

class CallNode;

class ClassLiteralExpressionNode;

class ClassLiteralFieldNode;

class ClassNode;

class ContinueNode;

class DeclarationNode;

class EmptyListNode;

class ForNode;

class FunctionNode;

class IdNode;

class IfNode;

class ListNode;

class MemberNode;

class FloatNode;

class NoneNode;

class NumberNode;

class ReturnNode;

class BoolOpNode;

class StringNode;

class SubscriptNode;

class TernaryNode;

class TupleNode;

class WhileNode;

class Node {
public:
    NodeType ntype;
    int start;
    int end;
    int line;
    int column;

    std::string node_type_string(NodeType type) const;

    virtual ~Node() = default;

    virtual bool equal(const Node& other) const = 0;

    bool operator==(const Node& other) const;
    bool operator!=(const Node& other) const;

    friend std::ostream& operator<<(std::ostream& os, const Node& node);

    void throw_dereference_error(NodeType expected) const;

    virtual AssignmentNode& assign();
    virtual BinopNode& binop();
    virtual BoolOpNode& boolop();
    virtual const BoolOpNode& boolop() const;
    virtual BlockNode& block();
    virtual BooleanNode& boolean();
    virtual BreakNode& brk();
    virtual CallNode& call();
    virtual ClassLiteralExpressionNode& clsexp();
    virtual ClassLiteralFieldNode& clsfld();
    virtual ClassNode& cls();
    virtual ContinueNode& cntinue();
    virtual DeclarationNode& decl();
    virtual EmptyListNode& emptylst();
    virtual FloatNode& flot();
    virtual ForNode& forloop();
    virtual FunctionNode& func();
    virtual IdNode& id();
    virtual IfNode& iff();
    virtual ListNode& lst();
    virtual MemberNode& member();
    virtual NoneNode& none();
    virtual NumberNode& number();
    virtual ReturnNode& retrn();
    virtual StringNode& strng();
    virtual SubscriptNode& sub();
    virtual TupleNode& tuple();
    virtual TernaryNode& ternary();
    virtual WhileNode& whil();
    virtual const AssignmentNode& assign() const;
    virtual const BinopNode& binop() const;
    virtual const BlockNode& block() const;
    virtual const BooleanNode& boolean() const;
    virtual const BreakNode& brk() const;
    virtual const CallNode& call() const;
    virtual const ClassLiteralExpressionNode& clsexp() const;
    virtual const ClassLiteralFieldNode& clsfld() const;
    virtual const ClassNode& cls() const;
    virtual const ContinueNode& cntinue() const;
    virtual const DeclarationNode& decl() const;
    virtual const EmptyListNode& emptylst() const;
    virtual const FloatNode& flot() const;
    virtual const ForNode& forloop() const;
    virtual const FunctionNode& func() const;
    virtual const IdNode& id() const;
    virtual const IfNode& iff() const;
    virtual const ListNode& lst() const;
    virtual const MemberNode& member() const;
    virtual const NoneNode& none() const;
    virtual const NumberNode& number() const;
    virtual const ReturnNode& retrn() const;
    virtual const StringNode& strng() const;
    virtual const SubscriptNode& sub() const;
    virtual const TernaryNode& ternary() const;
    virtual const TupleNode& tuple() const;
    virtual const WhileNode& whil() const;

    virtual json to_json() const;
};

#endif //NODE_H
