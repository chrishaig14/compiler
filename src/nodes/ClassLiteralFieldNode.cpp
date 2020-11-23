//
// Created by chris on 6/8/20.
//

#include "ClassLiteralFieldNode.h"

ClassLiteralFieldNode::ClassLiteralFieldNode(ObjectTypeNode* type, const std::unordered_map<std::string, Node*>& init)
        : type(type), init(init) {
    this->ntype = CLSFLD;
}

bool ClassLiteralFieldNode::equal(const Node& x) const {
    auto& other = x.clsfld();
    if (this->init.size() != other.init.size()) {
        return false;
    }
    for (auto i: this->init) {
        auto other_it = other.init.find(i.first);
        if (other_it == other.init.end()) {
            return false;
        }
        if (*i.second != *other_it->second) {
            return false;
        }
    }
    return *this->type == *other.type;
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
