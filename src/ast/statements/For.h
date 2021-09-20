//
// Created by chris on 14/8/20.
//

#ifndef FORNODE_H
#define FORNODE_H


#include "Statement.h"
#include "Block.h"
#include "../../util/utils.h"
#include "../ast.h"

class ast::For : public ast::Statement {
    ast::UExpNode _exp;
    ast::UBlock _body;
public:
    ExpNode& exp;
    std::string var;
    ast::Block& body;
    bool equal(const ast::Statement& other) const override;
    For(const std::string& var, ast::UExpNode exp, ast::UBlock body, TextPosition start, TextPosition end);
    ~For() override;

    nlohmann::json to_json() const override;

};


#endif //FORNODE_H
