//
// Created by chris on 29/8/20.
//

#ifndef TERNARYNODE_H
#define TERNARYNODE_H


#include "ExpNode.h"
#include "../../ast.h"


class ast::Ternary : public ast::ExpNode {
public:
    ExpNode* expression;
    ExpNode* true_case;
    ExpNode* false_case;

    Ternary(ExpNode* expression, ast::ExpNode* trueCase, ast::ExpNode* falseCase, TextPosition start, TextPosition end);
    bool equal(const ast::ExpNode& other) const override;

    ~Ternary();

};


#endif //TERNARYNODE_H
