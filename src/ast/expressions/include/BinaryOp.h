//
// Created by chris on 1/8/20.
//

#ifndef BINOPNODE_H
#define BINOPNODE_H


#include "ExpNode.h"
#include <util/utils.h>
#include "../../optypes.h"
#include "../../ast.h"

class ast::BinaryOp : public ast::ExpNode {
    ast::UExpNode _left;
    ast::UExpNode _right;
public:
    ExpNode& left;
    ExpNode& right;
    OpType op;
    TextPosition op_pos;

    BinaryOp(OpType op, ast::UExpNode left, ast::UExpNode right, TextPosition start, TextPosition end);

    static UBinaryOp make(OpType op, ast::UExpNode left, ast::UExpNode right, TextPosition start, TextPosition end) {
        return std::make_unique<BinaryOp>(op, std::move(left), std::move(right), start, end);
    }

    bool equal(const ast::ExpNode& x) const override;

    ~BinaryOp();
    nlohmann::json to_json() const override;

    ast::Type* ltype;
};


#endif //BINOPNODE_H
