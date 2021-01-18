//
// Created by chris on 1/8/20.
//

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
    for (int i = 0; i < this->parameter_types.size(); i++) {
        if (*this->parameter_types[i] != *other.parameter_types[i]) {
            return false;
        }
    }
    for (int i = 0; i < this->parameter_names.size(); i++) {
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

FunctionNode::FunctionNode(std::string identifier, VectorOfStrings parameter_names,
                           VectorOfTypes parameter_types, TypeNode* return_type, BlockNode* body)
        : body(body), return_type(return_type) {
    for (auto p: parameter_types) {
        assert(p != nullptr);
    }
    assert(return_type != nullptr);
    assert(body != nullptr);
    this->ntype = FUNC;
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

json FunctionNode::to_json() const {
    json j;
    j["node"] = "function";
//        j["identifier"] = this->identifier;
//        j["parameter_names"] = {};
//        j["parameter_types"] = {};
//        for (auto pn: this->parameter_names) {
//            j["parameter_names"].push_back(pn);
//        }
//        for (auto pt: this->parameter_types) {
//            j["parameter_types"].push_back(pt->to_json());
//        }
//        j["body"] = this->body->to_json();
//        j["return_type"] = this->return_type->to_json();
    return j;
}
