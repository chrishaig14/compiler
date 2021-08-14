//
// Created by chris on 17/8/20.
//

#include "BooleanNode.h"

BooleanNode::BooleanNode(bool value, TextPosition start, TextPosition end) : Node(NodeType::BOOLEAN, start, end),value(value) { }

bool BooleanNode::operator!=(const BooleanNode& other) const { return !(*this == other); }

bool BooleanNode::equal(const Node& x) const {
    auto& other = (BooleanNode&)x;
    return this->value == other.value;
}

nlohmann::json BooleanNode::to_json() const {
    nlohmann::json j;
    j["type"] = "boolean";
    j["boolean"]["value"] = this->value;
    return j;
}
