//
// Created by chris on 1/8/20.
//

#ifndef IFNODE_H
#define IFNODE_H


#include <vector>
#include "Node.h"

#include "Block.h"
#include "Node.h"
#include "ast.h"

class ast::If : public ast::Node {
public:
    UNode condition;
    std::unique_ptr<ast::Block> then;
    std::unique_ptr<ast::Block> selse;
    std::vector<std::pair<UNode, UBlockNode>> elifs;
    If(UNode& condition, std::unique_ptr<ast::Block>& then, std::vector<std::pair<UNode, UBlockNode>> elifs,
       std::unique_ptr<ast::Block>& selse, TextPosition start, TextPosition end);

    bool equal(const ast::Node& other) const override;

    nlohmann::json to_json() const override;

    ~If() override;

};


#endif //IFNODE_H
