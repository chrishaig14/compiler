//
// Created by chris on 1/8/20.
//

#ifndef DECLARATIONNODE_H
#define DECLARATIONNODE_H


#include <string>
#include "Node.h"
#include "TypeNode.h"

//#include "Node.h"
#include "../utils.h"

class DeclarationNode : public Node {
public:
    std::string identifier;
    TypeNode* type;
    Node* expression;

    DeclarationNode(const std::string& identifier, TypeNode* type, Node* expression) : DeclarationNode(
            identifier,
            type,
            expression,
            {-1, -1}
    ) {}

    DeclarationNode(const std::string& identifier, TypeNode* type, Node* expression, TextPosition start);

    bool equal(const Node& other) const override;

    ~DeclarationNode();

    json to_json() const override;

    DeclarationNode& decl() override;
    const DeclarationNode& decl() const override;

};


#endif //DECLARATIONNODE_H
