//
// Created by chris on 7/11/20.
//

#include "ContinueNode.h"

bool ContinueNode::equal(const Node& other) const {
    return true;
}

ContinueNode::ContinueNode(TextPosition start, TextPosition end) : Node(NodeType::CNTINUE, start, end) {
}

nlohmann::json ContinueNode::to_json() const {
    return {{"type", "continue"}};
}

