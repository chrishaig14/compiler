//
// Created by chris on 6/8/20.
//

#include "ClassLiteralFieldNode.h"

ClassLiteralFieldNode::ClassLiteralFieldNode(ObjectTypeNode* type, const std::map<std::string, Node*>& init)
        : type(type), init(init) {
    this->ntype = CLSFLD;
}

bool ClassLiteralFieldNode::equal(const Node& x) const {
    auto& other = x.clsfld();
    return this->type == other.type && this->init == other.init;
}

ClassLiteralFieldNode::~ClassLiteralFieldNode() {
    delete this->type;
    for (auto p: this->init) {
        delete p.second;
    }
}

ClassLiteralFieldNode& ClassLiteralFieldNode::clsfld() {
    return *this;
}

const ClassLiteralFieldNode& ClassLiteralFieldNode::clsfld() const {
    return *this;
}
