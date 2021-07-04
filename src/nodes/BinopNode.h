//
// Created by chris on 1/8/20.
//

#ifndef BINOPNODE_H
#define BINOPNODE_H


#include "Node.h"

#include "Node.h"
#include "../utils.h"
#include "optypes.h"


class BinopNode : public Node {
public:
    Node* left;
    Node* right;
    OpType op;
    TextPosition op_pos;

    BinopNode(OpType op, Node* left, Node* right, TextPosition start, TextPosition end);

    BinopNode(OpType op, Node* left, Node* right, TextPosition start);
    bool equal(const Node& x) const override;

    ~BinopNode();
    nlohmann::json to_json() override;

    TypeNode* ltype;
};


#endif //BINOPNODE_H
