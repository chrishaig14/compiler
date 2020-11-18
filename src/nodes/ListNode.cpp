//
// Created by chris on 1/8/20.
//

#include "ListNode.h"
#include "../utils.h"

void ListNode::accept(Visitor& visitor) {
    visitor.visit(*this);

}

ListNode::ListNode(VectorOfNodes elements) : elements(elements) {
    this->ntype = LST;

}

bool ListNode::equal(const Node& other) const {
    if (this->elements.size() != other.lst().elements.size()) {
        return false;
    }
    for (int i = 0; i < this->elements.size(); ++i) {
        if (*this->elements[i] != *other.lst().elements[i]) {
            return false;
        }
    }
    return this->elements == other.lst().elements;
}

ListNode& ListNode::lst() {
    return *this;
}

const ListNode& ListNode::lst() const {
    return *this;
}

