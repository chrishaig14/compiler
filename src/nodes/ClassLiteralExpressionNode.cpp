//
// Created by chris on 6/8/20.
//

#include "ClassLiteralExpressionNode.h"
#include "../utils.h"
void ClassLiteralExpressionNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}


ClassLiteralExpressionNode::ClassLiteralExpressionNode(ObjectTypeNode* type, const VectorOfNodes& init)
        : type(type), init(init) {}

bool ClassLiteralExpressionNode::operator!=(const ClassLiteralExpressionNode& other) const {
    return !(*this == other);
}

bool ClassLiteralExpressionNode::operator==(const ClassLiteralExpressionNode& other) const {
    return this->type == other.type && this->init == other.init;
}
