//
// Created by chris on 29/8/20.
//

#ifndef TERNARYNODE_H
#define TERNARYNODE_H


#include "Node.h"
#include "Visitor.h"

class TernaryNode : public Node {
public:
    Node* expression;
    Node* true_case;
    Node* false_case;;

    TernaryNode(Node* expression, Node* trueCase, Node* falseCase);

    bool equal(const Node& other) const override;

    TernaryNode& ternary() override;

    ~TernaryNode();

    const TernaryNode& ternary() const override;

};


#endif //TERNARYNODE_H
