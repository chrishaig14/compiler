//
// Created by chris on 30/8/20.
//

#include "NoneNode.h"

bool NoneNode::equal(const Node& other) const {
    return true;
}

NoneNode::NoneNode(TextPosition start, TextPosition end) : Node(NodeType::NONE, start, end) {
}

nlohmann::json NoneNode::to_json() const {
    return {{"type", "none"}};
}

