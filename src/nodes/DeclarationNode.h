//
// Created by chris on 1/8/20.
//

#ifndef DECLARATIONNODE_H
#define DECLARATIONNODE_H


#include <string>
#include "Node.h"
#include "TypeNode.h"
#include "../utils.h"

class DeclarationNode : public Node {
public:
    std::string identifier;
    TypeNode* type;
    std::unique_ptr<Node> expression;
    TextPosition eq_pos;
    DeclarationNode(const std::string& identifier, TypeNode* type, std::unique_ptr<Node>& expression, TextPosition start,
                    TextPosition eq_pos, TextPosition end);


    DeclarationNode(const std::string& identifier, TypeNode* type, Node* expression, TextPosition start);

    bool equal(const Node& other) const override;

    ~DeclarationNode() override;

    nlohmann::json to_json() const override;

};


#endif //DECLARATIONNODE_H
