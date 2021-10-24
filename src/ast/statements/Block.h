//
// Created by chris on 2/8/20.
//

#ifndef BLOCKNODE_H
#define BLOCKNODE_H


#include <vector>
#include "Statement.h"

#include "Statement.h"
#include <util/types.h>
#include "../general/Type.h"
#include "../ast.h"

class ast::Block : public ast::Statement {
public:

    Block(ast::VectorOfNodesU nodes, TextPosition start, TextPosition end);

    static ast::UBlock make(ast::VectorOfNodesU nodes, TextPosition start, TextPosition end) {
        return std::make_unique<Block>(std::move(nodes), start, end);
    }


    ast::VectorOfNodesU nodes;

    bool equal(const ast::Statement& p) const override;


    ~Block();
    nlohmann::json to_json() const override;
    std::vector<std::pair<std::string, ast::Type*>> local_vars;
};


#endif //BLOCKNODE_H
