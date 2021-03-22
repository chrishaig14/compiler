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
    this->throw_dereference_error(WHIL);
}

PartialApplication& Node::partial() {
    this->throw_dereference_error(PARTIAL);
}

const PartialApplication& Node::partial() const {
    this->throw_dereference_error(PARTIAL);
}

const TupleNode& Node::tuple() const {
    this->throw_dereference_error(TUPLE);
}

const TernaryNode& Node::ternary() const {
    this->throw_dereference_error(TERNARY);
}

const SubscriptNode& Node::sub() const {
    this->throw_dereference_error(SUB);
}

const StringNode& Node::strng() const {
    this->throw_dereference_error(STRNG);
}

const ReturnNode& Node::retrn() const {
    this->throw_dereference_error(RETRN);
}

const NumberNode& Node::number() const {
    this->throw_dereference_error(NUMBER);
}

const NoneNode& Node::none() const {
    this->throw_dereference_error(NONE);
}

const MemberNode& Node::member() const {
    this->throw_dereference_error(MEMBER);
}

const ListNode& Node::lst() const {
    this->throw_dereference_error(LST);
}

const IfNode& Node::iff() const {
    this->throw_dereference_error(IFF);
}

const EmptyListNode& Node::emptylst() const {
    this->throw_dereference_error(EMPTYLST);
}

const ForNode& Node::forloop() const {
    this->throw_dereference_error(FORLOOP);
}

const FunctionNode& Node::func() const {
    this->throw_dereference_error(FUNC);
}

const IdNode& Node::id() const {
    this->throw_dereference_error(ID);
}

const DeclarationNode& Node::decl() const {
    this->throw_dereference_error(DECL);
}

const DictNode& Node::dict() const {
    this->throw_dereference_error(DICT);
}

const ContinueNode& Node::cntinue() const {
    this->throw_dereference_error(CNTINUE);
}

const ClassNode& Node::cls() const {
    this->throw_dereference_error(CLS);
}

AssignmentNode& Node::assign() {
    this->throw_dereference_error(ASSIGN);
}

BinopNode& Node::binop() {
    this->throw_dereference_error(BINOP);
}

BlockNode& Node::block() {
    this->throw_dereference_error(BLOCK);
}

BooleanNode& Node::boolean() {
    this->throw_dereference_error(BOOLEAN);
}

BreakNode& Node::brk() {
    this->throw_dereference_error(BRK);
}

CallNode& Node::call() {
    this->throw_dereference_error(CALL);
}

TupleNode& Node::tuple() {
    this->throw_dereference_error(TUPLE);
}

ClassNode& Node::cls() {
    this->throw_dereference_error(CLS);
}

ContinueNode& Node::cntinue() {
    this->throw_dereference_error(CNTINUE);
}

DeclarationNode& Node::decl() {
    this->throw_dereference_error(DECL);
}

DictNode& Node::dict() {
    this->throw_dereference_error(DICT);
}

EmptyDictNode& Node::emptydict() {
    this->throw_dereference_error(EMPTYDICT);
}


EmptyListNode& Node::emptylst() {
    this->throw_dereference_error(EMPTYLST);
}

ForNode& Node::forloop() {
    this->throw_dereference_error(FORLOOP);
}

FunctionNode& Node::func() {
    this->throw_dereference_error(FUNC);
}

IdNode& Node::id() {
    this->throw_dereference_error(ID);
}

IfNode& Node::iff() {
    this->throw_dereference_error(IFF);
}

ListNode& Node::lst() {
    this->throw_dereference_error(LST);
}

MemberNode& Node::member() {
    this->throw_dereference_error(MEMBER);
}

NoneNode& Node::none() {
    this->throw_dereference_error(NONE);
}

NumberNode& Node::number() {
    this->throw_dereference_error(NUMBER);
}

ReturnNode& Node::retrn() {
    this->throw_dereference_error(RETRN);
}

StringNode& Node::strng() {
    this->throw_dereference_error(STRNG);
}

SubscriptNode& Node::sub() {
    this->throw_dereference_error(SUB);
}

TernaryNode& Node::ternary() {
    this->throw_dereference_error(TERNARY);
}

WhileNode& Node::whil() {
    this->throw_dereference_error(WHIL);
}

FloatNode& Node::flot() {
    this->throw_dereference_error(FLOT);
}

const AssignmentNode& Node::assign() const {
    this->throw_dereference_error(ASSIGN);
}

const BinopNode& Node::binop() const {
    this->throw_dereference_error(BINOP);
}

const BlockNode& Node::block() const {
    this->throw_dereference_error(BLOCK);
}

const BooleanNode& Node::boolean() const {
    this->throw_dereference_error(BOOLEAN);
}

const FloatNode& Node::flot() const {
    this->throw_dereference_error(FLOT);
}

const BreakNode& Node::brk() const {
    this->throw_dereference_error(BRK);
}

const CallNode& Node::call() const {
    this->throw_dereference_error(CALL);
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
        case ASSIGN:
            return "ASSIGN";
        case BINOP:
            return "BINOP";
        case BOOLOP:
            return "BOOLOP";
        case BLOCK:
            return "BLOCK";
        case BOOLEAN:
            return "BOOLEAN";
        case BRK:
            return "BRK";
        case CALL:
            return "CALL";
        case CLS:
            return "CLS";
        case CNTINUE:
            return "CNTINUE";
        case DECL:
            return "DECL";
        case EMPTYLST:
            return "EMPTYLST";
        case FLOT:
            return "FLOT";
        case FORLOOP:
            return "FORLOOP";
        case FUNC:
            return "FUNC";
        case ID:
            return "ID";
        case IFF:
            return "IFF";
        case LST:
            return "LST";
        case MEMBER:
            return "MEMBER";
        case NONE:
            return "NONE";
        case NUMBER:
            return "NUMBER";
        case RETRN:
            return "RETRN";
        case STRNG:
            return "STRNG";
        case SUB:
            return "SUB";
        case TERNARY:
            return "TERNARY";
        case OTYPE:
            return "OTYPE";
        case FTYPE:
            return "FTYPE";
        case WHIL:
            return "WHIL";
        case TUPLE:
            return "TUPLE";
            break;
        case PARTIAL:
            return "PARTIAL";
            break;
        case DICT:
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
    this->throw_dereference_error(BOOLOP);
}

const BoolOpNode& Node::boolop() const {
    this->throw_dereference_error(BOOLOP);
}

const EmptyDictNode& Node::emptydict() const {
    this->throw_dereference_error(EMPTYDICT);
}

const ImportNode& Node::import() const {
    this->throw_dereference_error(IMPORT);
}

ImportNode& Node::import() {
    this->throw_dereference_error(IMPORT);
}
