//
// Created by chris on 1/8/20.
//

#include "ReturnNode.h"

ReturnNode::ReturnNode(Node* expression, TextPosition start, TextPosition end) : Node(RETRN, start, end),
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

json ReturnNode::to_json() const {
    json j;
    j["node"] = "return";
//        j["expression"] = this->expression->to_json();
    return j;
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
