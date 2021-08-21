//
// Created by chris on 1/8/20.
//

#include "SubscriptNode.h"

SubscriptNode::SubscriptNode(UNode& parent, VectorOfNodesU& child, TextPosition start, TextPosition end)
        : Node(NodeType::SUB, start, end), parent(std::move(parent)), child(std::move(child)) {
}


bool SubscriptNode::equal(const Node& x) const {
    auto& other = (SubscriptNode&) x;
    if (this->child.size() != other.child.size()) {
        return false;
    }
    for (size_t i = 0; i < this->child.size(); i++) {
        if (*this->child[i] != *other.child[i]) {
            return false;
        }
    }
    return *this->parent == *other.parent;
}

SubscriptNode::~SubscriptNode() {
    // delete this->parent;
    // for (auto c: this->child) {
    //     delete c;
    // }
}

nlohmann::json SubscriptNode::to_json() const {
    nlohmann::json j;
    j["type"] = "subscript";
    j["subscript"]["parent"] = this->parent->to_json();
    j["subscript"]["child"] = this->child[0]->to_json();
    return j;
}

