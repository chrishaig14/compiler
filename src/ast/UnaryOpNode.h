//
// Created by chris on 9/5/21.
//

#ifndef XLANG_UNARYOPNODE_H
#define XLANG_UNARYOPNODE_H


#include "Node.h"
#include "ast.h"

enum UnaryOp {
    NOT
};

class ast::UnaryOpNode : public ast::Node {
public:
    bool equal(const ast::Node& other) const override;

    UnaryOpNode(UnaryOp op, ast::Node* exp, TextPosition start, TextPosition end);
    nlohmann::json to_json() const override;
    Node* exp;
    UnaryOp op;
};


#endif //XLANG_UNARYOPNODE_H
