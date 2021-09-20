//
// Created by chris on 1/8/20.
//

#ifndef SUBSCRIPTNODE_H
#define SUBSCRIPTNODE_H


#include "../../Node.h"

#include "../../Node.h"

#include "../../../types.h"
#include "../../ast.h"
#include "ExpNode.h"

class ast::Subscript : public ast::ExpNode {

public:
    Subscript(ast::UExpNode& parent, ast::VectorOfExpNodesU& child, TextPosition start, TextPosition end);
    ast::Type* parent_t;
    ast::UExpNode parent;
    ast::VectorOfExpNodesU child;

    bool equal(const ast::ExpNode& other) const override;

    ~Subscript();
    nlohmann::json to_json() const override;


};


#endif //SUBSCRIPTNODE_H
