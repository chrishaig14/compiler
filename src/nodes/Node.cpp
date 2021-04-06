//
// Created by chris on 1/8/20.
//

#include "Node.h"

bool Node::operator==(const Node& other) const {
    if (this->ntype != other.ntype) {
        return false;
    }
    return this->equal(other);
}

bool Node::operator!=(const Node& other) const {
    return !(this->operator==(other));
}

#pragma GCC diagnostic ignored "-Wreturn-type"

const WhileNode& Node::whil() const {
    this->throw_dereference_error(NodeType::WHIL);
}

PartialApplication& Node::partial() {
    this->throw_dereference_error(NodeType::PARTIAL);
}

const PartialApplication& Node::partial() const {
    this->throw_dereference_error(NodeType::PARTIAL);
}

const TupleNode& Node::tuple() const {
    this->throw_dereference_error(NodeType::TUPLE);
}

const TernaryNode& Node::ternary() const {
    this->throw_dereference_error(NodeType::TERNARY);
}

const SubscriptNode& Node::sub() const {
    this->throw_dereference_error(NodeType::SUB);
}

const StringNode& Node::strng() const {
    this->throw_dereference_error(NodeType::STRNG);
}

const ReturnNode& Node::retrn() const {
    this->throw_dereference_error(NodeType::RETRN);
}

const NumberNode& Node::number() const {
    this->throw_dereference_error(NodeType::NUMBER);
}

const NoneNode& Node::none() const {
    this->throw_dereference_error(NodeType::NONE);
}

const MemberNode& Node::member() const {
    this->throw_dereference_error(NodeType::MEMBER);
}

const ListNode& Node::lst() const {
    this->throw_dereference_error(NodeType::LST);
}

const IfNode& Node::iff() const {
    this->throw_dereference_error(NodeType::IFF);
}

const EmptyListNode& Node::emptylst() const {
    this->throw_dereference_error(NodeType::EMPTYLST);
}

const ForNode& Node::forloop() const {
    this->throw_dereference_error(NodeType::FORLOOP);
}

const FunctionNode& Node::func() const {
    this->throw_dereference_error(NodeType::FUNC);
}

const IdNode& Node::id() const {
    this->throw_dereference_error(NodeType::ID);
}

const DeclarationNode& Node::decl() const {
    this->throw_dereference_error(NodeType::DECL);
}

const DictNode& Node::dict() const {
    this->throw_dereference_error(NodeType::DICT);
}

const ContinueNode& Node::cntinue() const {
    this->throw_dereference_error(NodeType::CNTINUE);
}

const CastNode& Node::cast() const {
    this->throw_dereference_error(NodeType::CAST);
}

const ClassNode& Node::cls() const {
    this->throw_dereference_error(NodeType::CLS);
}

AssignmentNode& Node::assign() {
    this->throw_dereference_error(NodeType::ASSIGN);
}

BinopNode& Node::binop() {
    this->throw_dereference_error(NodeType::BINOP);
}

BlockNode& Node::block() {
    this->throw_dereference_error(NodeType::BLOCK);
}

BooleanNode& Node::boolean() {
    this->throw_dereference_error(NodeType::BOOLEAN);
}

BreakNode& Node::brk() {
    this->throw_dereference_error(NodeType::BRK);
}

CallNode& Node::call() {
    this->throw_dereference_error(NodeType::CALL);
}

TupleNode& Node::tuple() {
    this->throw_dereference_error(NodeType::TUPLE);
}

ClassNode& Node::cls() {
    this->throw_dereference_error(NodeType::CLS);
}

ContinueNode& Node::cntinue() {
    this->throw_dereference_error(NodeType::CNTINUE);
}

CastNode& Node::cast() {
    this->throw_dereference_error(NodeType::CAST);
}

DeclarationNode& Node::decl() {
    this->throw_dereference_error(NodeType::DECL);
}

DictNode& Node::dict() {
    this->throw_dereference_error(NodeType::DICT);
}

EmptyDictNode& Node::emptydict() {
    this->throw_dereference_error(NodeType::EMPTYDICT);
}

DefaultConstructorNode& Node::defconst() {
    this->throw_dereference_error(NodeType::DEF_CONST);
}

const DefaultConstructorNode& Node::defconst() const {
    this->throw_dereference_error(NodeType::DEF_CONST);
}


EmptyListNode& Node::emptylst() {
    this->throw_dereference_error(NodeType::EMPTYLST);
}

ForNode& Node::forloop() {
    this->throw_dereference_error(NodeType::FORLOOP);
}

FunctionNode& Node::func() {
    this->throw_dereference_error(NodeType::FUNC);
}

IdNode& Node::id() {
    this->throw_dereference_error(NodeType::ID);
}

IfNode& Node::iff() {
    this->throw_dereference_error(NodeType::IFF);
}

ListNode& Node::lst() {
    this->throw_dereference_error(NodeType::LST);
}

MemberNode& Node::member() {
    this->throw_dereference_error(NodeType::MEMBER);
}

NoneNode& Node::none() {
    this->throw_dereference_error(NodeType::NONE);
}

NumberNode& Node::number() {
    this->throw_dereference_error(NodeType::NUMBER);
}

ReturnNode& Node::retrn() {
    this->throw_dereference_error(NodeType::RETRN);
}

StringNode& Node::strng() {
    this->throw_dereference_error(NodeType::STRNG);
}

SubscriptNode& Node::sub() {
    this->throw_dereference_error(NodeType::SUB);
}

TernaryNode& Node::ternary() {
    this->throw_dereference_error(NodeType::TERNARY);
}

WhileNode& Node::whil() {
    this->throw_dereference_error(NodeType::WHIL);
}

FloatNode& Node::flot() {
    this->throw_dereference_error(NodeType::FLOT);
}

const AssignmentNode& Node::assign() const {
    this->throw_dereference_error(NodeType::ASSIGN);
}

const MethodNode& Node::method() const {
    this->throw_dereference_error(NodeType::METHOD);
}

MethodNode& Node::method() {
    this->throw_dereference_error(NodeType::METHOD);
}

const BinopNode& Node::binop() const {
    this->throw_dereference_error(NodeType::BINOP);
}

const BlockNode& Node::block() const {
    this->throw_dereference_error(NodeType::BLOCK);
}

const BooleanNode& Node::boolean() const {
    this->throw_dereference_error(NodeType::BOOLEAN);
}

const FloatNode& Node::flot() const {
    this->throw_dereference_error(NodeType::FLOT);
}

const BreakNode& Node::brk() const {
    this->throw_dereference_error(NodeType::BRK);
}

const CallNode& Node::call() const {
    this->throw_dereference_error(NodeType::CALL);
}

#pragma GCC diagnostic pop

void Node::throw_dereference_error(NodeType expected) const {
    throw std::runtime_error(
            "Getting " + this->node_type_string(expected) + " but it's a " + this->node_type_string(this->ntype));
}

std::ostream& operator<<(std::ostream& os, const Node& node) {
    return os << node.to_json();
}

std::string Node::node_type_string(NodeType type) const {
    switch (type) {
        case NodeType::ASSIGN:
            return "ASSIGN";
        case NodeType::BINOP:
            return "BINOP";
        case NodeType::BOOLOP:
            return "BOOLOP";
        case NodeType::BLOCK:
            return "BLOCK";
        case NodeType::BOOLEAN:
            return "BOOLEAN";
        case NodeType::BRK:
            return "BRK";
        case NodeType::CALL:
            return "CALL";
        case NodeType::CLS:
            return "CLS";
        case NodeType::CNTINUE:
            return "CNTINUE";
        case NodeType::DECL:
            return "DECL";
        case NodeType::EMPTYLST:
            return "EMPTYLST";
        case NodeType::FLOT:
            return "FLOT";
        case NodeType::FORLOOP:
            return "FORLOOP";
        case NodeType::FUNC:
            return "FUNC";
        case NodeType::ID:
            return "ID";
        case NodeType::IFF:
            return "IFF";
        case NodeType::LST:
            return "LST";
        case NodeType::MEMBER:
            return "MEMBER";
        case NodeType::NONE:
            return "NONE";
        case NodeType::NUMBER:
            return "NUMBER";
        case NodeType::RETRN:
            return "NodeType::RETRN";
        case NodeType::STRNG:
            return "STRNG";
        case NodeType::SUB:
            return "SUB";
        case NodeType::TERNARY:
            return "TERNARY";
        case NodeType::OTYPE:
            return "OTYPE";
        case NodeType::FTYPE:
            return "FTYPE";
        case NodeType::WHIL:
            return "WHIL";
        case NodeType::TUPLE:
            return "TUPLE";
            break;
        case NodeType::PARTIAL:
            return "PARTIAL";
            break;
        case NodeType::DICT:
            return "DICT";
            break;
    }
    return "UNKNOWN";
}

json Node::to_json() const {
    json j;
    j["name"] = "johnny";
    return j;
}

BoolOpNode& Node::boolop() {
    this->throw_dereference_error(NodeType::BOOLOP);
}

const BoolOpNode& Node::boolop() const {
    this->throw_dereference_error(NodeType::BOOLOP);
}

const EmptyDictNode& Node::emptydict() const {
    this->throw_dereference_error(NodeType::EMPTYDICT);
}

const ImportNode& Node::import() const {
    this->throw_dereference_error(NodeType::IMPORT);
}

ImportNode& Node::import() {
    this->throw_dereference_error(NodeType::IMPORT);
}
