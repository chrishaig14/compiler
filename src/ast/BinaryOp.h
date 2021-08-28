//
// Created by chris on 1/8/20.
//

#ifndef BINOPNODE_H
#define BINOPNODE_H


#include "Node.h"

#include "Node.h"
#include "../utils.h"
#include "optypes.h"
#include "ast.h"

class ast::BinaryOp : public ast::Node {
public:
    UNode left;
    UNode right;
    OpType op;
    TextPosition op_pos;

    BinaryOp(OpType op, UNode& left, UNode& right, TextPosition start, TextPosition end);

    static UBinopNode make(OpType op, UNode& left, UNode& right, TextPosition start, TextPosition end) {
        return std::make_unique<BinaryOp>(op, left, right, start, end);
    }

    bool equal(const ast::Node& x) const override;

    ~BinaryOp();
    nlohmann::json to_json() const override;

    TypeNode* ltype;
};


#endif //BINOPNODE_H
