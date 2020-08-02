//
// Created by chris on 1/8/20.
//

#include "ListNode.h"

void ListNode::accept(Visitor& visitor) {
    visitor.visit(*this);

}

ListNode::ListNode(std::vector<Node*> elements) : elements(elements) {

}

bool ListNode::equal(Node* other) const {
    auto other_ptr = dynamic_cast<ListNode*>(other);
    if (other_ptr == nullptr) return false;
    return *this == *other_ptr;
}

bool ListNode::operator==(ListNode& other) const {
    if (this->elements.size() != other.elements.size()) return false;
    for (int i = 0; i < this->elements.size(); i++) {
        if (!this->elements[i]->equal(other.elements[i])) return false;
    }
    return true;
}
