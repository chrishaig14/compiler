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

    std::string node_type_string(NodeType type) const {
        switch (type) {

            case ASSIGN:
                return "ASSIGN";
            case BINOP:
                return "BINOP";
            case BLOCK:
                return "BLOCK";

                break;
            case BOOLEAN:
                return "BOOLEAN";
                break;
            case BRK:
                return "BRK";
                break;
            case CALL:
                return "CALL";
                break;
            case CLSEXP:
                return "CLSEXP";
                break;
            case CLSFLD:
                return "CLSFLD";
                break;
            case CLS:
                return "CLS";
                break;
            case CNTINUE:
                break;
            case DECL:
                break;
            case EMPTYLST:
                break;
            case FORLOOP:
                break;
            case FUNC:
                break;
            case ID:
                return "ID";
                break;
            case IFF:
                break;
            case INSTANCE:
                break;
            case LST:
                break;
            case MEMBER:
                break;
            case NONE:
                break;
            case NUMBER:
                return "NUMBER";
                break;
            case RETRN:
                break;
            case STRNG:
                break;
            case STRCT:
                break;
            case SUB:
                break;
            case TERNARY:
                break;
            case OTYPE:
                break;
            case FTYPE:
                break;
            case WHIL:
                break;
            case UNINITIALIZED:
                break;
        }
        return "UNKNOWN";
    }

    virtual void accept(Visitor& visitor) = 0;

    virtual ~Node() = default;

    virtual bool equal(const Node& other) const = 0;

    bool operator==(const Node& other) const;
    bool operator!=(const Node& other) const;

    friend std::ostream& operator<<(std::ostream& os, const Node& node) {
        return os << node.to_json();
    }

    virtual AssignmentNode& assign() {
        throw std::runtime_error("Getting AssignmentNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual BinopNode& binop() {
        throw std::runtime_error("Getting BinopNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual BlockNode& block() {
        throw std::runtime_error("Getting BlockNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual BooleanNode& boolean() {
        throw std::runtime_error("Getting BooleanNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual BreakNode& brk() {
        throw std::runtime_error("Getting BreakNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual CallNode& call() {
        throw std::runtime_error("Getting CallNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual ClassLiteralFieldNode& clsfld() {
        throw std::runtime_error("Getting ClassLiteralFieldNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual ClassLiteralExpressionNode& clsexp() {
        throw std::runtime_error(
                "Getting ClassLiteralExpressionNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual ClassNode& cls() {
        throw std::runtime_error("Getting ClassNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual ContinueNode& cntinue() {
        throw std::runtime_error("Getting ContinueNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual DeclarationNode& decl() {
        throw std::runtime_error("Getting DeclarationNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual EmptyListNode& emptylst() {
        throw std::runtime_error("Getting EmptyListNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual ForNode& forloop() {
        throw std::runtime_error("Getting ForNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual FunctionNode& func() {
        throw std::runtime_error("Getting FunctionNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual IdNode& id() {
        throw std::runtime_error("Getting IdNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual IfNode& iff() {
        throw std::runtime_error("Getting IfNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual ListNode& lst() {
        throw std::runtime_error("Getting ListNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual MemberNode& member() {
        throw std::runtime_error("Getting MemberNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual NoneNode& none() {
        throw std::runtime_error("Getting NoneNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual NumberNode& number() {
        throw std::runtime_error("Getting NumberNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual ReturnNode& retrn() {
        throw std::runtime_error("Getting ReturnNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual StringNode& strng() {
        throw std::runtime_error("Getting StringNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual SubscriptNode& sub() {
        throw std::runtime_error("Getting SubscriptNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual TernaryNode& ternary() {
        throw std::runtime_error("Getting TernaryNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual WhileNode& whil() {
        throw std::runtime_error("Getting WhileNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual const AssignmentNode& assign() const {
        throw std::runtime_error("Getting AssignmentNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual const BinopNode& binop() const {
        throw std::runtime_error("Getting BinopNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual const BlockNode& block() const {
        throw std::runtime_error("Getting BlockNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual const BooleanNode& boolean() const {
        throw std::runtime_error("Getting BooleanNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual const BreakNode& brk() const {
        throw std::runtime_error("Getting BreakNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual const CallNode& call() const {
        throw std::runtime_error("Getting CallNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual const ClassLiteralFieldNode& clsfld() const {
        throw std::runtime_error("Getting ClassLiteralFieldNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual const ClassLiteralExpressionNode& clsexp() const {
        throw std::runtime_error(
                "Getting ClassLiteralExpressionNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual const ClassNode& cls() const {
        throw std::runtime_error("Getting ClassNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual const ContinueNode& cntinue() const {
        throw std::runtime_error("Getting ContinueNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual const DeclarationNode& decl() const {
        throw std::runtime_error("Getting DeclarationNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual const EmptyListNode& emptylst() const {
        throw std::runtime_error("Getting EmptyListNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual const ForNode& forloop() const {
        throw std::runtime_error("Getting ForNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual const FunctionNode& func() const {
        throw std::runtime_error("Getting FunctionNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual const IdNode& id() const {
        throw std::runtime_error("Getting IdNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual const IfNode& iff() const {
        throw std::runtime_error("Getting IfNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual const ListNode& lst() const {
        throw std::runtime_error("Getting ListNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual const MemberNode& member() const {
        throw std::runtime_error("Getting MemberNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual const NoneNode& none() const {
        throw std::runtime_error("Getting NoneNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual const NumberNode& number() const {
        throw std::runtime_error("Getting NumberNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual const ReturnNode& retrn() const {
        throw std::runtime_error("Getting ReturnNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual const StringNode& strng() const {
        throw std::runtime_error("Getting StringNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual const SubscriptNode& sub() const {
        throw std::runtime_error("Getting SubscriptNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual const TernaryNode& ternary() const {
        throw std::runtime_error("Getting TernaryNode but it's a " + this->node_type_string(this->ntype));
    }

    virtual const WhileNode& whil() const {
        throw std::runtime_error("Getting WhileNode but it's a " + this->node_type_string(this->ntype));
    }


    virtual json to_json() const {
        json j;
        j["name"] = "johnny";
        return j;
    }
};

#endif //NODE_H
