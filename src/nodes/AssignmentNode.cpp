//
// Created by chris on 1/8/20.
//

#include "AssignmentNode.h"

AssignmentNode::AssignmentNode(Node* lvalue, Node* rvalue, TextPosition start, TextPosition end) : Node(NodeType::ASSIGN,
                                                                                                        start,
                                                                                                        end),
                                                                                                   lvalue(lvalue),
                                                                                                   rvalue(rvalue) {
}

bool AssignmentNode::equal(const Node& x) const {
    auto& other = x.assign();
    return *this->lvalue == *other.lvalue && *this->rvalue == *other.rvalue;
}

AssignmentNode::~AssignmentNode() {
    delete this->lvalue;
    delete this->rvalue;
}

json AssignmentNode::to_json() const {
    json j;
    j["node"] = "assignment";
//        j["lvalue"] = this->lvalue->to_json();
//        j["rvalue"] = this->rvalue->to_json();
    return j;
}
