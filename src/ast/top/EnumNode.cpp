//
// Created by chris on 29/4/21.
//

#include "EnumNode.h"

using namespace ast;

EnumNode::EnumNode(std::string id, VectorOfStrings values, TextPosition start, TextPosition end) : ast::TopNode(
        TopNodeType::ENUM,
        start,
        end) {
    this->id = id;
    this->values = values;

}

bool EnumNode::equal(const ast::TopNode& other) const {
    return false;
}

nlohmann::json EnumNode::to_json() const {
    return nlohmann::json();
}
