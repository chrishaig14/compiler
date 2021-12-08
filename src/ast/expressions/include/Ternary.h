//
// Created by chris on 29/8/20.
//

#ifndef TERNARYNODE_H
#define TERNARYNODE_H


#include "ExpNode.h"
#include "../../ast.h"


class ast::Ternary : public ast::ExpNode {
public:
    ast::UExpNode expression;
    ast::UExpNode true_case;
    ast::UExpNode false_case;

    Ternary(ast::UExpNode expression, ast::UExpNode trueCase, ast::UExpNode falseCase, TextPosition start,
            TextPosition end);

    bool equal(const ast::ExpNode& other) const override;

    nlohmann::json to_json() const override;

};


#endif //TERNARYNODE_H
