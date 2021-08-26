//
// Created by chris on 1/8/20.
//

#ifndef IFNODE_H
#define IFNODE_H


#include <vector>
#include "Node.h"

#include "Block.h"
#include "Node.h"

class IfNode : public Node {
public:
    UNode condition;
    std::unique_ptr<ast::Block> then;
    std::unique_ptr<ast::Block> selse;
    std::vector<std::pair<Node*, ast::Block*>> elifs;
    IfNode(UNode& condition, std::unique_ptr<ast::Block>& then,
           std::vector<std::pair<Node*, ast::Block*>> elifs, std::unique_ptr<ast::Block>& selse, TextPosition start,
           TextPosition end);

    bool equal(const Node& other) const override;

    nlohmann::json to_json() const override;

    ~IfNode() override;

};


#endif //IFNODE_H
