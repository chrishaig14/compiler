//
// Created by chris on 14/8/20.
//

#ifndef FORNODE_H
#define FORNODE_H


#include "Node.h"
#include "Block.h"
#include "../utils.h"
#include "ast.h"

class ast::For : public Node {
public:
    std::string var;
    UNode exp;
    std::unique_ptr<ast::Block> body;
    bool equal(const Node& other) const override;
    For(const std::string& var, UNode& exp, std::unique_ptr<ast::Block>& body, TextPosition start, TextPosition end);
    ~For() override;

    nlohmann::json to_json() const override;

};


#endif //FORNODE_H
