//
// Created by chris on 9/5/21.
//

#ifndef XLANG_UNARYOPNODE_H
#define XLANG_UNARYOPNODE_H


#include "Node.h"

enum UnaryOp {
    NOT
};

class UnaryOpNode : public Node {
public:
    bool equal(const Node& other) const override;

    UnaryOpNode(UnaryOp op, Node* exp, TextPosition start, TextPosition end);
    nlohmann::json to_json() override;
    Node* exp;
    UnaryOp op;
};


#endif //XLANG_UNARYOPNODE_H
