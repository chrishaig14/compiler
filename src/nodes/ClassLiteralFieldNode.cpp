//
// Created by chris on 6/8/20.
//

#include "ClassLiteralFieldNode.h"
#include "../utils.h"
void ClassLiteralFieldNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

ClassLiteralFieldNode::ClassLiteralFieldNode(ObjectTypeNode& type, const std::map<std::string, NodeContainer>& init)
        : type(type), init(init) {}

bool ClassLiteralFieldNode::operator!=(const ClassLiteralFieldNode& other) const { return !(*this == other); }

bool ClassLiteralFieldNode::operator==(const ClassLiteralFieldNode& other) const {
    return this->type == other.type && this->init == other.init;
}
