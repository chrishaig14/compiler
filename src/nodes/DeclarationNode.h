//
// Created by chris on 1/8/20.
//

#ifndef DECLARATIONNODE_H
#define DECLARATIONNODE_H


#include <string>
#include "Node.h"
#include "TypeNode.h"
#include "Visitor.h"
//#include "Node.h"
#include "../utils.h"
class DeclarationNode : public Node {
public:
    std::string identifier;
    TypeNode* type;
    Node* expression;

    DeclarationNode(const std::string& identifier, TypeNode* type, Node* expression);

    bool equal(const Node& other) const override ;

    ~DeclarationNode() {
        if (this->type != nullptr) delete this->type;
        if (this->expression != nullptr) delete this->expression;
    }

    json to_json() const override {
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

    DeclarationNode& decl() override;
    const DeclarationNode& decl() const override;

};


#endif //DECLARATIONNODE_H
