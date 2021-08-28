//
// Created by chris on 9/5/21.
//

#ifndef XLANG_UNARYOP_H
#define XLANG_UNARYOP_H


#include "Node.h"
#include "ast.h"

enum UnaryOpType {
    NOT
};

class ast::UnaryOp : public ast::Node {
public:
    bool equal(const ast::Node& other) const override;

    UnaryOp(UnaryOpType op, ast::Node* exp, TextPosition start, TextPosition end);
    nlohmann::json to_json() const override;
    Node* exp;
    UnaryOpType op;
};


#endif //XLANG_UNARYOP_H
