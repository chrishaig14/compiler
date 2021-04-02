//
// Created by chris on 1/8/20.
//

#include "ListNode.h"

ListNode::ListNode(VectorOfNodes elements, TextPosition start, TextPosition end) :Node(LST, start, end), elements(elements) {
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
    return true;
}

ListNode& ListNode::lst() {
    return *this;
}

const ListNode& ListNode::lst() const {
    return *this;
}

ListNode::~ListNode() {
    for (auto e: this->elements) {
        delete e;
    }
}

json ListNode::to_json() const {
    json j;
    j["node"] = "list";
    j["elements"] = {};
//        for (auto e: this->elements) {
//            j["elements"].push_back(e->to_json());
//        }
    return j;
}

