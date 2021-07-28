//
// Created by chris on 1/8/20.
//

#include "AssignmentNode.h"
#include "../json/json.hpp"
AssignmentNode::AssignmentNode(Node* lvalue, Node* rvalue, TextPosition start, TextPosition end)
        : Node(NodeType::ASSIGN, start, end), lvalue(lvalue), rvalue(rvalue) {
}

bool AssignmentNode::equal(const Node& x) const {
    auto& other = (AssignmentNode&) x;
    return *this->lvalue == *other.lvalue && *this->rvalue == *other.rvalue;
}

AssignmentNode::~AssignmentNode() {
    // delete this->lvalue;
    // delete this->rvalue;
}

nlohmann::json AssignmentNode::to_json() {
    nlohmann::json j;
    j["type"] = "assignment";
    j["assignment"]["lvalue"] = this->lvalue->to_json();
    j["assignment"]["rvalue"] = this->rvalue->to_json();
    return j;
}

