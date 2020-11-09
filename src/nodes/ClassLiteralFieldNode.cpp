//
// Created by chris on 6/8/20.
//

#include "ClassLiteralFieldNode.h"

void ClassLiteralFieldNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

bool ClassLiteralFieldNode::equal(NodeContainer other) const {
//    const ClassLiteralFieldNode* other_ptr = dynamic_cast<const ClassLiteralFieldNode*>(other);
//    if (other_ptr == nullptr) return false;
//    if (!this->type->equal(other_ptr->type)) return false;
//    if (this->init.size() != other_ptr->init.size()) return false;
//    for (auto f: this->init) {
//        if (other_ptr->init.count(f.first) == 0) return false;
//        if (!f.second->equal(other_ptr->init.at(f.first))) return false;
//    }
//    return true;
return false;
}

ClassLiteralFieldNode::ClassLiteralFieldNode(ObjectTypeNode* type, const std::map<std::string, NodeContainer>& init)
        : type(type), init(init) {}
