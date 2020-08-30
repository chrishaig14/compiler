//
// Created by chris on 29/8/20.
//

#ifndef UNTITLED1_TERNARYNODE_H
#define UNTITLED1_TERNARYNODE_H


#include "Node.h"
#include "Visitor.h"

class TernaryNode : public Node {
public:
    Node* expression;
    Node* true_case;
    Node* false_case;;

    TernaryNode(Node* expression, Node* trueCase, Node* falseCase);

    void accept(Visitor& visitor) override;

    bool equal(Node* other) const override;

    bool operator==(TernaryNode& other) const;

};


#endif //UNTITLED1_TERNARYNODE_H
