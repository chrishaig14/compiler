//
// Created by chris on 1/8/20.
//

#ifndef RETURNNODE_H
#define RETURNNODE_H


#include "Node.h"
#include "../types.h"
#include "TypeNode.h"
#include "ast.h"

class ast::Return : public ast::Node {
public:
    explicit Return(UNode& expression, TextPosition start, TextPosition end);


    bool equal(const ast::Node& other) const override;

    ~Return();
    nlohmann::json to_json() const override;

    UNode expression;
    std::vector<std::pair<std::string, TypeNode*>> reachables;
};

#endif //RETURNNODE_H
