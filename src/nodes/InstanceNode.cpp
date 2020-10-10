//
// Created by chris on 4/10/20.
//

#include "InstanceNode.h"

void InstanceNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

bool InstanceNode::equal(Node* other) const {
    return false;
}

InstanceNode::InstanceNode(const std::string& className, const std::string& typeName,
                           const std::vector<FunctionNode*>& functions) : class_name(className), type_name(typeName),
                                                                          functions(functions) {}
