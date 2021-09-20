//
// Created by chris on 1/8/20.
//

#ifndef IFNODE_H
#define IFNODE_H


#include <vector>
#include "../Node.h"

#include "Block.h"
#include "../Node.h"
#include "../exp/include/ExpNode.h"
#include "../ast.h"

class ast::If : public ast::Node {
    ast::UExpNode _condition;
    ast::UBlock _then;
    std::vector<std::pair<ast::UExpNode, ast::UBlock>> _elifs;
public:
    ast::UBlock selse;
    ast::Block& then;
    ExpNode& condition;
    std::vector<std::pair<RExpNode, std::reference_wrapper<ast::Block>>> elifs;
    If(ast::UExpNode condition, std::unique_ptr<ast::Block> then, std::vector<std::pair<ast::UExpNode, ast::UBlock>> elifs,
       std::unique_ptr<ast::Block> selse, TextPosition start, TextPosition end);

    bool equal(const ast::Node& other) const override;

    nlohmann::json to_json() const override;

    ~If() override;

};


#endif //IFNODE_H
