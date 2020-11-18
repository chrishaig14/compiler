//
// Created by chris on 1/8/20.
//

#ifndef NODE_H
#define NODE_H

#include "json.hpp"
#include "node_types.h"
#include <string>

using json = nlohmann::json;

class Visitor;

class AssignmentNode;

class BinopNode;

class BlockNode;

class BooleanNode;

class BreakNode;

class CallNode;

class ClassLiteralFieldNode;

class ClassLiteralExpressionNode;

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

class NoneNode;

class NumberNode;

class ReturnNode;

class StringNode;

class SubscriptNode;

class TernaryNode;

class WhileNode;

class Node {
public:
    NodeType ntype;
    int start;
    int end;
    int line;
    int column;

    virtual void accept(Visitor& visitor) = 0;

    virtual ~Node() = default;

    friend std::ostream& operator<<(std::ostream& os, const Node& node) {
        return os << node.to_json();
    }

    AssignmentNode& assign() { throw std::runtime_error("Getting assign but it's not the correct type!"); }

    BinopNode& binop() { throw std::runtime_error("Getting assign but it's not the correct type!"); }

    BlockNode& block() { throw std::runtime_error("Getting assign but it's not the correct type!"); }

    BooleanNode& boolean() { throw std::runtime_error("Getting assign but it's not the correct type!"); }

    BreakNode& brk() { throw std::runtime_error("Getting assign but it's not the correct type!"); }

    CallNode& call() { throw std::runtime_error("Getting assign but it's not the correct type!"); }

    ClassLiteralFieldNode& clsfld() { throw std::runtime_error("Getting assign but it's not the correct type!"); }

    ClassLiteralExpressionNode& clsexp() {
        throw std::runtime_error("Getting assign but it's not the correct type!");
    }

    ClassNode& cls() { throw std::runtime_error("Getting assign but it's not the correct type!"); }

    ContinueNode& cntinue() { throw std::runtime_error("Getting assign but it's not the correct type!"); }

    DeclarationNode& decl() { throw std::runtime_error("Getting assign but it's not the correct type!"); }

    EmptyListNode& emptylst() { throw std::runtime_error("Getting assign but it's not the correct type!"); }

    ForNode& forloop() { throw std::runtime_error("Getting assign but it's not the correct type!"); }

    FunctionNode& func() { throw std::runtime_error("Getting assign but it's not the correct type!"); }

    IdNode& id() { throw std::runtime_error("Getting assign but it's not the correct type!"); }

    IfNode& iff() { throw std::runtime_error("Getting assign but it's not the correct type!"); }

    ListNode& lst() { throw std::runtime_error("Getting assign but it's not the correct type!"); }

    MemberNode& member() { throw std::runtime_error("Getting assign but it's not the correct type!"); }

    NoneNode& none() { throw std::runtime_error("Getting assign but it's not the correct type!"); }

    NumberNode& number() { throw std::runtime_error("Getting assign but it's not the correct type!"); }

    ReturnNode& retrn() { throw std::runtime_error("Getting assign but it's not the correct type!"); }

    StringNode& strng() { throw std::runtime_error("Getting assign but it's not the correct type!"); }

    SubscriptNode& sub() { throw std::runtime_error("Getting assign but it's not the correct type!"); }

    TernaryNode& ternary() { throw std::runtime_error("Getting assign but it's not the correct type!"); }

    WhileNode& whil() { throw std::runtime_error("Getting assign but it's not the correct type!"); }

    virtual json to_json() const {
        json j;
        j["name"] = "johnny";
        return j;
    }
};

#endif //NODE_H
