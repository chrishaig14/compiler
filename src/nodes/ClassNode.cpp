//
// Created by chris on 4/10/20.
//

#include "ClassNode.h"

ClassNode::ClassNode(const std::string& className, VectorOfStrings type_parameters,
                     MapStringType members,
                     std::unordered_map<std::string, FunctionNode*> functions, TextPosition start, TextPosition end)
        :Node(CLS, start, end), methods(functions), class_name(className), members(members) {
    this->type_parameters = type_parameters;
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
