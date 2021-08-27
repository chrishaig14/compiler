//
// Created by chris on 29/8/20.
//

#ifndef TERNARYNODE_H
#define TERNARYNODE_H


#include "Node.h"
#include "ast.h"


class ast::TernaryNode : public ast::Node {
public:
    Node* expression;
    Node* true_case;
    Node* false_case;

    TernaryNode(Node* expression, ast::Node* trueCase, ast::Node* falseCase, TextPosition start, TextPosition end);
    bool equal(const ast::Node& other) const override;

    ~TernaryNode();

};


#endif //TERNARYNODE_H
