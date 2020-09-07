//
// Created by chris on 6/8/20.
//

#include "ClassLiteralExpressionNode.h"

void ClassLiteralExpressionNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}


bool ClassLiteralExpressionNode::equal(Node* other) const {
    const ClassLiteralExpressionNode* other_ptr = dynamic_cast<const ClassLiteralExpressionNode*>(other);
    if (other_ptr == nullptr) return false;
    bool result = true;
    if (this->init.size() != other_ptr->init.size()) return false;
    for (int i = 0; i < this->init.size(); i++) {

    }
    return result;
}

ClassLiteralExpressionNode::ClassLiteralExpressionNode(ObjectTypeNode* type, const std::vector<Node*>& init)
        : type(type), init(init) {}
