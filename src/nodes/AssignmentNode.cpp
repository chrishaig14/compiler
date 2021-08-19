//
// Created by chris on 1/8/20.
//

#include "AssignmentNode.h"
#include "../json/json.hpp"

AssignmentNode::AssignmentNode(std::unique_ptr<Node>& lvalue, std::unique_ptr<Node>& rvalue, TextPosition start,
                               TextPosition end) : Node(NodeType::ASSIGN, start, end), lvalue(std::move(lvalue)),
                                                   rvalue(std::move(rvalue)) {
}

bool AssignmentNode::equal(const Node& x) const {
    auto& other = (AssignmentNode&) x;
    return *this->lvalue == *other.lvalue && *this->rvalue == *other.rvalue;
}

AssignmentNode::~AssignmentNode() {
    // delete this->lvalue;
    // delete this->rvalue;
}

nlohmann::json AssignmentNode::to_json() const {
    nlohmann::json j;
    j["type"] = "assignment";
    j["assignment"]["lvalue"] = this->lvalue->to_json();
    j["assignment"]["rvalue"] = this->rvalue->to_json();
    return j;
}

