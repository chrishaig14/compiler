//
// Created by chris on 25/11/20.
//

#ifndef UNTITLED1_BOOLOPNODE_H
#define UNTITLED1_BOOLOPNODE_H

#include "Node.h"
#include "optypes.h"
#include "TypeNode.h"
#include "ast.h"


class ast::BoolOp : public ast::Node {
public:
    UNode left;
    UNode right;
    BoolOpType op;

    BoolOp(BoolOpType op, UNode& left, UNode& right, TextPosition start, TextPosition end);
    bool equal(const ast::Node& x) const override;

    ~BoolOp();
    nlohmann::json to_json() const override;

    TypeNode* ltype;
    TypeNode* rtype;
};

#endif //UNTITLED1_BOOLOPNODE_H
