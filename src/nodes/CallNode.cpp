//
// Created by chris on 1/8/20.
//

#include "CallNode.h"

void CallNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

CallNode::CallNode(Node* function, const std::vector<Node*>& arguments) : function(function), arguments(arguments) {}

bool CallNode::equal(Node* other) const {
    auto other_ptr = dynamic_cast<CallNode*>(other);
    if (other_ptr == nullptr) return false;
    return *this == *other_ptr;
}

bool CallNode::operator==(CallNode& other) const {
    if (!this->function->equal(other.function)) return false;
    if (this->arguments.size() != other.arguments.size()) return false;
    for (int i = 0; i < this->arguments.size(); i++) {
        if (!this->arguments[i]->equal(other.arguments[i])) return false;
    }
    return true;
}
