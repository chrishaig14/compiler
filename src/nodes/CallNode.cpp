//
// Created by chris on 1/8/20.
//

#include "CallNode.h"
#include "../utils.h"
void CallNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

CallNode::CallNode(NodeContainer function, const std::vector<NodeContainer>& arguments) : function(function),
                                                                                          arguments(arguments) {}

bool CallNode::operator==(const CallNode& other) const {
    return this->function == other.function && this->arguments == other.arguments;
}

bool CallNode::operator!=(const CallNode& other) const { return !(*this == other); }
