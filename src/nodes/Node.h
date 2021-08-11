//
// Created by chris on 1/8/20.
//

#ifndef NODE_H
#define NODE_H

#include "node_types.h"
#include <string>
#include "../scanner/TextPosition.h"
#include "../json/json_fwd.hpp"
#include <iostream>

class AssignmentNode;

class BinopNode;

class BlockNode;

class BooleanNode;

class BreakNode;

class CallNode;

class ClassNode;

class ContinueNode;

class DeclarationNode;

class DictNode;

class EmptyListNode;

class ForNode;

class FunctionNode;

class IdNode;

class IfNode;

class ListNode;

class EmptyDictNode;

class MemberNode;

class FloatNode;

class NoneNode;

class NumberNode;

class ReturnNode;

class PartialApplication;

class BoolOpNode;

class StringNode;

class SubscriptNode;

class DefaultConstructorNode;

class TernaryNode;

class TupleNode;

class WhileNode;

class ImportNode;

class CastNode;

class MethodNode;

class Node {
public:
    const NodeType ntype;

    TextPosition start;
    TextPosition end;

    Node(NodeType ntype, TextPosition start, TextPosition end) : ntype(ntype) {
        this->start = start;
        this->end = end;
    }

    std::string node_type_string(NodeType type) const;

    virtual ~Node() {
        std::cout << "Destructor for " << this << " called" << std::endl;
    };

    virtual bool equal(const Node& other) const {
        std::cout << "BASE NODE for " << this << " EQUAL CALLED!" << std::endl;
        return false;
    };

    bool operator==(const Node& other) const;
    bool operator!=(const Node& other) const;

    virtual nlohmann::json to_json() = 0;
};

#endif //NODE_H
