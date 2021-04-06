//
// Created by chris on 6/9/20.
//

#include "EmptyListNode.h"


EmptyListNode::EmptyListNode(TypeNode* type, TextPosition start, TextPosition end) : Node(NodeType::EMPTYLST, start, end),
                                                                                     type(type) {
}

EmptyListNode& EmptyListNode::emptylst() {
    return *this;
}

const EmptyListNode& EmptyListNode::emptylst() const {
    return *this;
}

bool EmptyListNode::equal(const Node& other) const {
    return *other.emptylst().type == *this->type;
}

EmptyListNode::~EmptyListNode() {
    delete this->type;
}
