//
// Created by chris on 1/8/20.
//

#include <stdexcept>
#include "Node.h"

#include "../json/json.hpp"

bool Node::operator==(const Node& other) const {
    if (this->ntype != other.ntype) {
        return false;
    }
    return this->equal(other);
}

bool Node::operator!=(const Node& other) const {
    return !(this->operator==(other));
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
        default:
            return "UNKNOWN";
    }
}

