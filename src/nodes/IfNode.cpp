//
// Created by chris on 1/8/20.
//

#include "IfNode.h"

bool IfNode::equal(const Node& x) const {
    auto& other = x.iff();
    if (*this->condition != *other.condition) {
        return false;
    }
    if (*this->then != *other.then) {
        return false;
    }
    if (this->elifs.size() != other.elifs.size()) {
        return false;
    }
    for (size_t i = 0; i < this->elifs.size(); i++) {
        if (*this->elifs[i].first != *other.elifs[i].first) {
            return false;
        }
        if (*this->elifs[i].second != *other.elifs[i].second) {
            return false;
        }
    }
    if (this->selse != nullptr && other.selse == nullptr || this->selse == nullptr && other.selse != nullptr) {
        return false;
    }
    return this->selse == nullptr && other.selse == nullptr || *this->selse == *other.selse;

}

IfNode::IfNode(Node* condition, BlockNode* then,
               std::vector<std::pair<Node*, BlockNode*>> elifs, BlockNode* selse, TextPosition start, TextPosition end) : Node(NodeType::IFF, start, end),condition(condition),
                                                                                    then(then), selse(selse),
                                                                                    elifs(elifs) {
    assert(condition != nullptr);
    assert(then != nullptr);
}

IfNode& IfNode::iff() {
    return *this;
}

const IfNode& IfNode::iff() const {
    return *this;
}

IfNode::~IfNode() {
    delete this->condition;
    delete this->then;
    if (this->selse != nullptr) {
        delete this->selse;
    }
    for (auto p: this->elifs) {
        delete p.first;
        delete p.second;
    }
}

json IfNode::to_json() const {
    json j;
    j["node"] = "if";
//        j["condition"] = this->condition->to_json();
//        j["then"] = this->then->to_json();
    return j;
}

