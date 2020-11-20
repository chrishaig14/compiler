//
// Created by chris on 1/8/20.
//

#include "CallNode.h"
#include "../utils.h"

CallNode::CallNode(Node* function, const VectorOfNodes& arguments) : function(function),
                                                                     arguments(arguments) {this->ntype = CALL;}

bool CallNode::equal(const Node& x) const {
    auto& other = x.call();
    if (this->arguments.size() != other.arguments.size()) {
        return false;
    }
    for (int i = 0; i < this->arguments.size(); ++i) {
        if (*this->arguments[i] != *other.arguments[i]) {
            return false;
        }
    }
    return *this->function == *other.function;
}

CallNode& CallNode::call() {
    return *this;
}

const CallNode& CallNode::call() const {
    return *this;
}
