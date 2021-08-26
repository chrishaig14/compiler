//
// Created by chris on 1/8/20.
//

#ifndef RETURNNODE_H
#define RETURNNODE_H


#include "Node.h"
#include "../types.h"
#include "TypeNode.h"
#include "ast.h"

class ast::ReturnNode : public Node {
public:
    explicit ReturnNode(UNode& expression, TextPosition start, TextPosition end);


    bool equal(const Node& other) const override;

    ~ReturnNode();
    nlohmann::json to_json() const override;

    UNode expression;
    std::vector<std::pair<std::string, TypeNode*>> reachables;
};

#endif //RETURNNODE_H
