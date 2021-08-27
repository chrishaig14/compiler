//
// Created by chris on 6/9/20.
//

#include "EmptyListNode.h"

using namespace ast;
EmptyListNode::EmptyListNode(TypeNode* type, TextPosition start, TextPosition end) : ast::Node(NodeType::EMPTYLST,
                                                                                          start,
                                                                                          end), type(type) {
}

bool EmptyListNode::equal(const ast::Node& other) const {
    return *((EmptyListNode&) other).type == *this->type;
}

EmptyListNode::~EmptyListNode() {
    // delete this->type;
}

nlohmann::json EmptyListNode::to_json() const {
    return {{"type",       "empty_list"},
            {"empty_list", {{"type", this->type->to_json()}}}};
}
