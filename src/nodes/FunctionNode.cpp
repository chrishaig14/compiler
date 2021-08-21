//
// Created by chris on 1/8/20.
//

#include <cassert>
#include "FunctionNode.h"

bool FunctionNode::equal(const Node& x) const {
    const auto& other = (FunctionNode&) x;
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

FunctionNode::FunctionNode(std::string identifier, VectorOfStrings parameter_names,
                           const VectorOfTypes& parameter_types, TypeNode* return_type,
                           std::unique_ptr<BlockNode>& body, TextPosition start, TextPosition end)
        : Node(NodeType::FUNC, start, end), body(std::move(body)), return_type(return_type) {
    for (auto p: parameter_types) {
        assert(p != nullptr);
    }
    this->implicit = nullptr;
    // assert(return_type != nullptr);
    // assert(body != nullptr);
    this->identifier = identifier;
    this->parameter_names = parameter_names;
    this->parameter_types = parameter_types;
}

FunctionNode::~FunctionNode() {
    // for (auto* pt: this->parameter_types) {
    //     delete pt;
    // }
    // delete this->body;
    // delete this->return_type;
}

nlohmann::json FunctionNode::to_json() const {
    std::vector<nlohmann::json> params;
    for (size_t i = 0; i < this->parameter_names.size(); i++) {
        params.push_back({{"id",   this->parameter_names[i]},
                          {"type", this->parameter_types[i]->to_json()}});
    }
    return {{"type",     "function"},
            {"function", {{"id", this->identifier}, {"parameters", params}, {"body", this->body->to_json()}, {"return_type", this->return_type->to_json()}}}};
}

