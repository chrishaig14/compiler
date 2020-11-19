//
// Created by chris on 1/8/20.
//

#include "IfNode.h"
#include "../utils.h"

void IfNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

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
    for (int i = 0; i < this->elifs.size(); i++) {
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
               std::vector<std::pair<Node*, BlockNode*>> elifs, BlockNode* selse) : condition(condition),
                                                                                    then(then), selse(selse),
                                                                                    elifs(elifs) {
    this->ntype = IFF;
}

IfNode& IfNode::iff() {
    return *this;
}

const IfNode& IfNode::iff() const {
    return *this;
}

