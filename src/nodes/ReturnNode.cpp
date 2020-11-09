//
// Created by chris on 1/8/20.
//

#include "ReturnNode.h"

void ReturnNode::accept(Visitor& visitor) {
    visitor.visit(*this);

}

ReturnNode::ReturnNode(NodeContainer expression) : expression(expression) {}

bool ReturnNode::equal(NodeContainer other) const {
//    auto other_ptr = dynamic_cast<ReturnNode*>(other);
//    if (other_ptr == nullptr) return false;
//    return *this == *other_ptr;
return false;
}

bool ReturnNode::operator==(ReturnNode& other) const {
//    return this->expression->equal(other.expression);
return false;
}