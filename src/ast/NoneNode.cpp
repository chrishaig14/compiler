//
// Created by chris on 30/8/20.
//

#include "NoneNode.h"
using namespace ast;
bool NoneNode::equal(const ast::Node& other) const {
    return true;
}

NoneNode::NoneNode(TextPosition start, TextPosition end) : ast::Node(NodeType::NONE, start, end) {
}

nlohmann::json NoneNode::to_json() const {
    return {{"type", "none"}};
}

