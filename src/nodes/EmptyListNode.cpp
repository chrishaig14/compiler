//
// Created by chris on 6/9/20.
//

#include "EmptyListNode.h"


EmptyListNode::EmptyListNode(TypeNode* type, TextPosition start, TextPosition end) : Node(NodeType::EMPTYLST,
                                                                                          start,
                                                                                          end), type(type) {
}

bool EmptyListNode::equal(const Node& other) const {
    return *((EmptyListNode&) other).type == *this->type;
}

EmptyListNode::~EmptyListNode() {
    delete this->type;
}

nlohmann::json EmptyListNode::to_json() {
    return {{"type",       "empty_list"},
            {"empty_list", {{"type", this->type->to_json()}}}};
}
