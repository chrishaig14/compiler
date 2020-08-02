//
// Created by chris on 1/8/20.
//

#include "ClassNode.h"

void ClassNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

ClassNode::ClassNode(std::string identifier, std::vector<std::string> templateParameters,
                     std::vector<DeclarationNode*> fields, std::vector<FunctionNode*> methods) : identifier(
        identifier),
                                                                                                 template_parameters(
                                                                                                         templateParameters),
                                                                                                 fields(fields),
                                                                                                 methods(methods) {}

