//
// Created by chris on 25/11/20.
//

#ifndef UNTITLED1_BOOLOPNODE_H
#define UNTITLED1_BOOLOPNODE_H

#include "Node.h"
#include "optypes.h"
#include "TypeNode.h"

class BoolOpNode : public Node {
public:
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    BoolOp op;

    BoolOpNode(BoolOp op, std::unique_ptr<Node>& left, std::unique_ptr<Node>& right, TextPosition start,
               TextPosition end);
    bool equal(const Node& x) const override;

    ~BoolOpNode();
    nlohmann::json to_json() const override;

    TypeNode* ltype;
    TypeNode* rtype;
};

#endif //UNTITLED1_BOOLOPNODE_H
