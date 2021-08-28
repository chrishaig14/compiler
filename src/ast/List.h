//
// Created by chris on 1/8/20.
//

#ifndef LISTNODE_H
#define LISTNODE_H


#include <vector>
#include "Node.h"
#include "../types.h"
#include "ast.h"
class ast::List : public ast::Node {
public:
    VectorOfNodesU elements;

    List(VectorOfNodesU& elements, TextPosition start, TextPosition end);

    bool equal(const ast::Node& other) const override;

    ~List() override;
    nlohmann::json to_json() const override;

    TypeNode* type;
};


#endif //LISTNODE_H
