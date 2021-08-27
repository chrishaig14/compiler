//
// Created by chris on 18/8/20.
//

#include "BreakNode.h"
using namespace ast;
bool BreakNode::equal(const ast::Node& other) const {
    return true;
}

BreakNode::BreakNode(TextPosition start, TextPosition end) : ast::Node(NodeType::BRK, start, end) {
}

nlohmann::json BreakNode::to_json() const {
    return {{"type", "break"}};
}


