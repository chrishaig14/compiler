//
// Created by chris on 7/11/20.
//

#include "ContinueNode.h"
using namespace ast;
using namespace ast;

bool ContinueNode::equal(const ast::Node& other) const {
    return true;
}

ContinueNode::ContinueNode(TextPosition start, TextPosition end) : ast::Node(NodeType::CNTINUE, start, end) {
}

nlohmann::json ContinueNode::to_json() const {
    return {{"type", "continue"}};
}

