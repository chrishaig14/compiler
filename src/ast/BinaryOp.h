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
    ast::UNode _left;
    ast::UNode _right;
public:
    Node& left;
    Node& right;
    OpType op;
    TextPosition op_pos;

    BinaryOp(OpType op, ast::UNode left, ast::UNode right, TextPosition start, TextPosition end);

    static UBinaryOp make(OpType op, ast::UNode left, ast::UNode right, TextPosition start, TextPosition end) {
        return std::make_unique<BinaryOp>(op, std::move(left), std::move(right), start, end);
    }

    bool equal(const ast::Node& x) const override;

    ~BinaryOp();
    nlohmann::json to_json() const override;

    ast::Type* ltype;
};


#endif //BINOPNODE_H
