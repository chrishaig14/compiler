//
// Created by chris on 4/10/20.
//

#include "ClassNode.h"

void ClassNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

bool ClassNode::equal(Node* other) const {
    return false;
}

ClassNode::ClassNode(const std::string& className, const std::string& typeName,
                     const std::map<std::string, FunctionTypeNode*>& functions)
        : functions(functions), class_name(className), type_name(typeName) {}
