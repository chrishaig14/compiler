//
// Created by chris on 16/8/20.
//

#include <cassert>
#include "WhileNode.h"

WhileNode::WhileNode(Node* condition, BlockNode* body, TextPosition start, TextPosition end) : Node(NodeType::WHIL,
                                                                                                    start,
                                                                                                    end), body(body),
                                                                                               condition(condition) {
    assert(condition != nullptr);
    assert(body != nullptr);
}

bool WhileNode::equal(const Node& x) const {
    const auto& other = (WhileNode&) x;
    if ((this->body == nullptr && other.body != nullptr) || (this->body != nullptr && other.body == nullptr)) {
        return false;
    }

    return *this->condition == *other.condition &&
           ((this->body == nullptr && other.body == nullptr) || *this->body == *other.body);
}

WhileNode::~WhileNode() {
    delete this->body;
    delete this->condition;
}

nlohmann::json WhileNode::to_json() {
    return {{"type",  "while"},
            {"while", {{"condition", this->condition->to_json()}, {"body", this->body->to_json()}}}};
}


