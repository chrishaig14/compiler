//
// Created by chris on 1/8/20.
//

#ifndef DECLARATIONNODE_H
#define DECLARATIONNODE_H


#include <string>
#include "Node.h"
#include "TypeNode.h"
#include "../utils.h"

class ast::Declaration : public ast::Node {
public:
    std::string identifier;
    UTypeNode type;
    UNode expression;
    TextPosition eq_pos;
    Declaration(const std::string& identifier, UTypeNode type, UNode expression, TextPosition start,
                TextPosition eq_pos, TextPosition end);


    Declaration(const std::string& identifier, UTypeNode type, ast::Node* expression, TextPosition start);

    bool equal(const ast::Node& other) const override;

    ~Declaration() override;

    nlohmann::json to_json() const override;

};


#endif //DECLARATIONNODE_H
