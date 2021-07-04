//
// Created by chris on 18/8/20.
//

#include "BreakNode.h"

bool BreakNode::equal(const Node& other) const {
    return true;
}

BreakNode::BreakNode(TextPosition start, TextPosition end) : Node(NodeType::BRK, start, end) {
}

nlohmann::json BreakNode::to_json() {
    return {{"type", "break"}};
}


