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
    UNode _condition;
    UBlockNode _then;
    std::vector<std::pair<UNode, UBlockNode>> _elifs;
public:
    UBlockNode selse;
    ast::Block& then;
    Node& condition;
    std::vector<std::pair<std::reference_wrapper<Node>, std::reference_wrapper<ast::Block>>> elifs;
    If(UNode condition, std::unique_ptr<ast::Block> then, std::vector<std::pair<UNode, UBlockNode>> elifs,
       std::unique_ptr<ast::Block> selse, TextPosition start, TextPosition end);

    bool equal(const ast::Node& other) const override;

    nlohmann::json to_json() const override;

    ~If() override;

};


#endif //IFNODE_H
