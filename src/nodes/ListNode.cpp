//
// Created by chris on 1/8/20.
//

#include "ListNode.h"
#include "../utils.h"
void ListNode::accept(Visitor& visitor) {
    visitor.visit(*this);

}

ListNode::ListNode(std::vector<NodeContainer> elements) : elements(elements) {

}

bool ListNode::operator!=(const ListNode& other) const { return !(*this == other); }

bool ListNode::operator==(const ListNode& other) const {
    return this->elements == other.elements;
}

