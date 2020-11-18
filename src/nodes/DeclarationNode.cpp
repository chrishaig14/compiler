//
// Created by chris on 1/8/20.
//

#include "DeclarationNode.h"

void DeclarationNode::accept(Visitor &visitor) {
    visitor.visit(*this);
}

DeclarationNode::DeclarationNode(const std::string &identifier, TypeNode *type, Node *expression) : identifier(
        identifier),
                                                                                                    type(type),
                                                                                                    expression(
                                                                                                            expression) { this->ntype = DECL; }

bool DeclarationNode::equal(const Node &x) const {
    auto &other = x.decl();
    return this->identifier == other.identifier && *this->expression == *other.expression &&
           ((this->type != nullptr && other.type !=
                                      nullptr && *this->type == *other.type) || (this->type ==
                                                                                 nullptr && other.type == nullptr));
}

DeclarationNode &DeclarationNode::decl() {
    return *this;
}

const DeclarationNode &DeclarationNode::decl() const {
    return *this;
}

