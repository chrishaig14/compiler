//
// Created by chris on 1/8/20.
//

#include "DeclarationNode.h"

DeclarationNode::DeclarationNode(const std::string& identifier, TypeNode* type, Node* expression, TextPosition start,
                                 TextPosition eq_pos, TextPosition end) : Node(NodeType::DECL, start, end),
                                                                          identifier(identifier), type(type),
                                                                          expression(expression) {
    this->eq_pos = eq_pos;
}

bool DeclarationNode::equal(const Node& x) const {
    auto& other = x.decl();
    return this->identifier == other.identifier && *this->expression == *other.expression &&
           ((this->type != nullptr && other.type != nullptr && *this->type == *other.type) ||
            (this->type == nullptr && other.type == nullptr));
}

DeclarationNode& DeclarationNode::decl() {
    return *this;
}

const DeclarationNode& DeclarationNode::decl() const {
    return *this;
}

DeclarationNode::~DeclarationNode() {
    if (this->type != nullptr) {
        delete this->type;
    }
    if (this->expression != nullptr) {
        delete this->expression;
    }
}


