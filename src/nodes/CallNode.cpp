//
// Created by chris on 1/8/20.
//

#include "CallNode.h"
#include "../utils.h"
void CallNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

CallNode::CallNode(Node* function, const VectorOfNodes& arguments) : function(function),
                                                                                          arguments(arguments) {}

bool CallNode::operator==(const CallNode& other) const {
    return this->function == other.function && this->arguments == other.arguments;
}

bool CallNode::operator!=(const CallNode& other) const { return !(*this == other); }
