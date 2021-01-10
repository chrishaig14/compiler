//
// Created by chris on 6/8/20.
//

#include "ClassLiteralFieldNode.h"

ClassLiteralFieldNode::ClassLiteralFieldNode(ObjectTypeNode* type, const std::unordered_map<std::string, Node*>& init)
        : type(type) {
    for (auto x: init) {
        this->init_names.push_back(x.first);
    }
    std::sort(this->init_names.begin(), this->init_names.end());

    for (int i = 0; i < this->init_names.size(); i++) {
        this->init_values.push_back(init.at(this->init_names[i]));
    }
    this->ntype = CLSFLD;
}

bool ClassLiteralFieldNode::equal(const Node& x) const {
    auto& other = x.clsfld();
    if (this->init_names.size() != other.init_names.size()) {
        return false;
    }
    for (int i = 0; i < this->init_names.size(); i++) {
        if (this->init_names[i] != other.init_names[i]) {
            return false;
        }
        if (*this->init_values[i] != *other.init_values[i]) {
            return false;
        }
    }
    return *this->type == *other.type;
}

ClassLiteralFieldNode::~ClassLiteralFieldNode() {
    delete this->type;
    for (auto p: this->init_values) {
        delete p;
    }
}

ClassLiteralFieldNode& ClassLiteralFieldNode::clsfld() {
    return *this;
}

const ClassLiteralFieldNode& ClassLiteralFieldNode::clsfld() const {
    return *this;
}
