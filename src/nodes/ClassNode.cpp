//
// Created by chris on 4/10/20.
//

#include "ClassNode.h"

ClassNode::ClassNode(const std::string& className, std::vector<std::string> type_parameters,
                     std::unordered_map<std::string, TypeNode*> members,
                     std::unordered_map<std::string, FunctionNode*> functions)
        : methods(functions), class_name(className), members(members) {
    this->type_parameters = type_parameters;
    this->ntype = CLS;
}

bool ClassNode::equal(const Node& other) const {
    return false;
}

ClassNode& ClassNode::cls() {
    return *this;
}

const ClassNode& ClassNode::cls() const {
    return *this;
}

ClassNode::~ClassNode() {
    for (auto mem: this->members) {
        delete mem.second;
    }
    for (auto method: this->methods) {
        delete method.second;
    }
}
