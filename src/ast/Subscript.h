//
// Created by chris on 1/8/20.
//

#ifndef SUBSCRIPTNODE_H
#define SUBSCRIPTNODE_H


#include "Node.h"

#include "Node.h"

#include "../types.h"
#include "ast.h"

class ast::Subscript : public ast::Node {

public:
    Subscript(UNode& parent, VectorOfNodesU& child, TextPosition start, TextPosition end);
    TypeNode* parent_t;
    UNode parent;
    VectorOfNodesU child;

    bool equal(const ast::Node& other) const override;

    ~Subscript();
    nlohmann::json to_json() const override;


};


#endif //SUBSCRIPTNODE_H
