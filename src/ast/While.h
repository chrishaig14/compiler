//
// Created by chris on 16/8/20.
//

#ifndef WHILENODE_H
#define WHILENODE_H


#include "Node.h"
#include "Block.h"
#include "ast.h"

class ast::While : public ast::Node {
public:
    std::unique_ptr<ast::Block> body;
    ast::UExpNode condition;

    While(ast::UExpNode& condition, std::unique_ptr<ast::Block>& body, TextPosition start, TextPosition end);

    bool equal(const ast::Node& other) const override;

    ~While() override;
    nlohmann::json to_json() const override;

};


#endif //WHILENODE_H
