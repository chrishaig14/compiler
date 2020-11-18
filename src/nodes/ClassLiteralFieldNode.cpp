//
// Created by chris on 6/8/20.
//

#include "ClassLiteralFieldNode.h"
#include "../utils.h"

void ClassLiteralFieldNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

ClassLiteralFieldNode::ClassLiteralFieldNode(ObjectTypeNode* type, const std::map<std::string, Node*>& init)
        : type(type), init(init) {}

bool ClassLiteralFieldNode::equal(const Node& x) const {
    auto& other = x.clsfld();
    return this->type == other.type && this->init == other.init;
}
