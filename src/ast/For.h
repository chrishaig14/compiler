//
// Created by chris on 14/8/20.
//

#ifndef FORNODE_H
#define FORNODE_H


#include "Node.h"
#include "Block.h"
#include "../utils.h"
#include "ast.h"

class ast::For : public ast::Node {
    ast::UNode _exp;
    ast::UBlock _body;
public:
    Node& exp;
    std::string var;
    ast::Block& body;
    bool equal(const ast::Node& other) const override;
    For(const std::string& var, ast::UNode exp, ast::UBlock body, TextPosition start, TextPosition end);
    ~For() override;

    nlohmann::json to_json() const override;

};


#endif //FORNODE_H
