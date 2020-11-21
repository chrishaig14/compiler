//
// Created by chris on 6/8/20.
//

#include "ClassLiteralExpressionNode.h"


ClassLiteralExpressionNode::ClassLiteralExpressionNode(ObjectTypeNode* type, const VectorOfNodes& init)
        : type(type), init(init) {
    this->ntype = CLSEXP;
}

bool ClassLiteralExpressionNode::equal(const Node& x) const {
    auto& other = x.clsexp();
    return this->type == other.type && this->init == other.init;
}

ClassLiteralExpressionNode::~ClassLiteralExpressionNode() {
    for (auto i: this->init) {
        delete i;
    }
    delete this->type;
}

ClassLiteralExpressionNode& ClassLiteralExpressionNode::clsexp() {
    return *this;
}

const ClassLiteralExpressionNode& ClassLiteralExpressionNode::clsexp() const {
    return *this;
}
