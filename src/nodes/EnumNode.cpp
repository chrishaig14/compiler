//
// Created by chris on 29/4/21.
//

#include "EnumNode.h"

EnumNode::EnumNode(std::string id, VectorOfStrings values, TextPosition start, TextPosition end) : Node(NodeType::ENUM,
                                                                                                        start,
                                                                                                        end) {
    this->id = id;
    this->values = values;

}

bool EnumNode::equal(const Node& other) const {
    return false;
}

nlohmann::json EnumNode::to_json() {
    return nlohmann::json();
}
