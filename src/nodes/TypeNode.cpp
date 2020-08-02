//
// Created by chris on 1/8/20.
//

#include "TypeNode.h"

void TypeNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

TypeNode::TypeNode(const std::string& identifier, const std::vector<TypeNode*>& typeParameters) : identifier(
        identifier), type_parameters(typeParameters) {}

json TypeNode::to_json() const {
    json j;
    j["node"] = "type";
    j["identifier"] = identifier;
    j["type_parameters"] = {};
    for (auto tp: this->type_parameters) {
        j["type_parameters"].push_back(tp->to_json());
    }
    return j;
}
