//
// Created by chris on 1/8/20.
//

#include <cassert>
#include "FunctionNode.h"

bool FunctionNode::equal(const Node& x) const {
    auto& other = x.func();
//    return false;
    if (this->identifier != other.identifier) {
        return false;
    }
    if (this->parameter_types.size() != other.parameter_types.size()) {
        return false;
    }
    if (this->parameter_names.size() != other.parameter_names.size()) {
        return false;
    }
    for (size_t i = 0; i < this->parameter_types.size(); i++) {
        if (*this->parameter_types[i] != *other.parameter_types[i]) {
            return false;
        }
    }
    for (size_t i = 0; i < this->parameter_names.size(); i++) {
        if (this->parameter_names[i] != other.parameter_names[i]) {
            return false;
        }
    }
    return *this->body == *other.body;
}

FunctionNode& FunctionNode::func() {
    return *this;
}

const FunctionNode& FunctionNode::func() const {
    return *this;
}

FunctionNode::FunctionNode(std::string identifier, VectorOfStrings parameter_names, VectorOfTypes parameter_types,
                           TypeNode* return_type, BlockNode* body, TextPosition start, TextPosition end) : Node(NodeType::FUNC,
                                                                                                                start,
                                                                                                                end),
                                                                                                           body(body),
                                                                                                           return_type(
                                                                                                                   return_type) {
    for (auto p: parameter_types) {
        assert(p != nullptr);
    }
    assert(return_type != nullptr);
    assert(body != nullptr);
    this->identifier = identifier;
    this->parameter_names = parameter_names;
    this->parameter_types = parameter_types;
}

FunctionNode::~FunctionNode() {
    for (auto pt: this->parameter_types) {
        delete pt;
    }
    delete this->body;
    delete this->return_type;
}

