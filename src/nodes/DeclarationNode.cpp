//
// Created by chris on 1/8/20.
//

#include "DeclarationNode.h"

DeclarationNode::DeclarationNode(const std::string& identifier, TypeNode* type, Node* expression, TextPosition start) : identifier(
        identifier),
                                                                                                    type(type),
                                                                                                    expression(
                                                                                                            expression) { this->ntype = DECL; this->start = start;}

bool DeclarationNode::equal(const Node& x) const {
    auto& other = x.decl();
    return this->identifier == other.identifier && *this->expression == *other.expression &&
           ((this->type != nullptr && other.type !=
                                      nullptr && *this->type == *other.type) || (this->type ==
                                                                                 nullptr && other.type == nullptr));
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

json DeclarationNode::to_json() const {
    json j;
    j["node"] = "declaration";
    j["identifier"] = this->identifier;
    j["expression"] = {};
//        if (this->expression != nullptr) {
//            j["expression"] = this->expression->to_json();
//        }
    if (this->type != nullptr) {
//            j["type"] = this->type->to_json();
    }
    return j;
}

