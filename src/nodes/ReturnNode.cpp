//
// Created by chris on 1/8/20.
//

#include "ReturnNode.h"

ReturnNode::ReturnNode(Node* expression, TextPosition start, TextPosition end) : Node(NodeType::RETRN, start, end),
                                                                                 expression(expression) {
}

bool ReturnNode::equal(const Node& x) const {
    return *x.retrn().expression == *this->expression;
}

ReturnNode& ReturnNode::retrn() {
    return *this;
}

const ReturnNode& ReturnNode::retrn() const {
    return *this;
}


ReturnNode::~ReturnNode() {
    if (this->ret_type != nullptr) {
        delete this->ret_type;
    }
    for (auto r: this->reachables) {
        delete r.second;
    }
    delete this->expression;
}
