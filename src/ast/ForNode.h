//
// Created by chris on 14/8/20.
//

#ifndef FORNODE_H
#define FORNODE_H


#include "Node.h"
#include "Block.h"
#include "../utils.h"

class ForNode : public Node {
public:
    std::string var;
    UNode exp;
    std::unique_ptr<ast::Block> body;
    bool equal(const Node& other) const override;
    ForNode(const std::string& var, UNode& exp, std::unique_ptr<ast::Block>& body, TextPosition start, TextPosition end);
    ~ForNode() override;

    nlohmann::json to_json() const override;

};


#endif //FORNODE_H
