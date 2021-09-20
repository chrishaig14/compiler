//
// Created by chris on 1/8/20.
//

#ifndef DECLARATIONNODE_H
#define DECLARATIONNODE_H


#include <string>
#include "Statement.h"
#include "../expressions/include/ExpNode.h"
#include "../general/Type.h"
#include "../../util/utils.h"

class ast::Declaration : public ast::Statement {
    ast::UExpNode _expression;
public:
    ExpNode& expression;
    std::string identifier;
    ast::UTypeNode type;
    TextPosition eq_pos;
    Declaration(const std::string& identifier, ast::UTypeNode type, ast::UExpNode expression, TextPosition start,
                TextPosition eq_pos, TextPosition end);


    Declaration(const std::string& identifier, ast::UTypeNode type, ast::Statement* expression, TextPosition start);

    bool equal(const ast::Statement& other) const override;

    ~Declaration() override;

    nlohmann::json to_json() const override;

};


#endif //DECLARATIONNODE_H
