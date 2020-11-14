//
// Created by chris on 4/10/20.
//

#include "ClassNode.h"

void ClassNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

bool ClassNode::equal(NodeContainer other) const {
    return false;
}

ClassNode::ClassNode(const std::string& className, std::vector<std::string> type_parameters,
                     std::map<std::string,TypeNode> members,
                     std::map<std::string, FunctionNode> functions)
        : methods(functions), class_name(className), members(members) {
    this->type_parameters = type_parameters;
}
