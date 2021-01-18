//
// Created by chris on 6/8/20.
//

#include "ClassLiteralExpressionNode.h"


ClassLiteralExpressionNode::ClassLiteralExpressionNode(ObjectType* type, const VectorOfNodes& init)
        : type(type), init(init) {
    this->ntype = CLSEXP;
}

bool ClassLiteralExpressionNode::equal(const Node& x) const {
    auto& other = x.clsexp();
    if (other.init.size() != this->init.size()) {
        return false;
    }
    for (int i = 0; i < this->init.size(); i++) {
        if (*this->init[i] != *other.init[i]) {
            return false;
        }
    }
    return *this->type == *other.type;
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
