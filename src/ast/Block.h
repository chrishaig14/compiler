//
// Created by chris on 2/8/20.
//

#ifndef BLOCKNODE_H
#define BLOCKNODE_H


#include <vector>
#include "Node.h"

#include "Node.h"
#include "../types.h"
#include "TypeNode.h"
#include "../units/Entity.h"
#include "ast.h"

class ast::Block : public ast::Node {
public:

    Block(ast::VectorOfNodesU nodes, TextPosition start, TextPosition end);

    static ast::UBlock make(ast::VectorOfNodesU nodes, TextPosition start, TextPosition end) {
        return std::make_unique<Block>(std::move(nodes), start, end);
    }


    ast::VectorOfNodesU nodes;

    bool equal(const ast::Node& p) const override;


    ~Block();
    nlohmann::json to_json() const override;
    std::vector<std::pair<std::string, ast::TypeNode*>> local_vars;
};


#endif //BLOCKNODE_H
