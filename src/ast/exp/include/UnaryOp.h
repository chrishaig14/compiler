//
// Created by chris on 9/5/21.
//

#ifndef XLANG_UNARYOP_H
#define XLANG_UNARYOP_H


#include "ExpNode.h"
#include "../../ast.h"

enum UnaryOpType {
    NOT
};

class ast::UnaryOp : public ast::ExpNode {
public:
    bool equal(const ast::ExpNode& other) const override;

    UnaryOp(UnaryOpType op, ast::UExpNode exp, TextPosition start, TextPosition end);
    nlohmann::json to_json() const override;
    ast::UExpNode exp;
    UnaryOpType op;
};


#endif //XLANG_UNARYOP_H
