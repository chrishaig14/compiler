//
// Created by chris on 16/8/20.
//

#ifndef WHILENODE_H
#define WHILENODE_H


#include "Node.h"
#include "Block.h"
#include "ast.h"

class ast::While : public Node {
public:
    std::unique_ptr<ast::Block> body;
    UNode condition;

    While(UNode& condition, std::unique_ptr<ast::Block>& body, TextPosition start, TextPosition end);

    bool equal(const Node& other) const override;

    ~While() override;
    nlohmann::json to_json() const override;

};


#endif //WHILENODE_H
