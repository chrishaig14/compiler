//
// Created by chris on 4/10/20.
//

#include "ClassNode.h"

ClassNode::ClassNode(const std::string& className, VectorOfStrings type_parameters, MapStringType members,
                     std::unordered_map<std::string, FunctionNode*> functions,
                     std::map<std::string, std::pair<TypeNode*, Node*>> static_members,
                     std::unordered_map<std::string, FunctionNode*> static_methods, TextPosition start,
                     TextPosition end) : Node(NodeType::CLS, start, end), members(members),
                                         static_members(static_members), methods(functions),
                                         static_methods(static_methods), class_name(className) {
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
    for (const auto& mem: this->members) {
        delete mem.second;
    }
    for (const auto& method: this->methods) {
        delete method.second;
    }
}
