//
// Created by chris on 31/1/21.
//

#include "DictNode.h"

DictNode& DictNode::dict() {
    return *this;
}

const DictNode& DictNode::dict() const {
    return *this;
}

bool DictNode::equal(const Node& other) const {
    auto& o = other.dict();
    if (this->items.size() != o.items.size()) {
        return false;
    }
    for (int i = 0; i < this->items.size(); i++) {
        auto p = this->items[i];
        auto op = o.items[i];
        if (*p.first != *op.first) {
            return false;
        }
        if (*p.second != *op.second) {
            return false;
        }
    }
    return true;
}
