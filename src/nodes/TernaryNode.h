//
// Created by chris on 29/8/20.
//

#ifndef TERNARYNODE_H
#define TERNARYNODE_H


#include "Node.h"
#include "Visitor.h"

class TernaryNode : public Node {
public:
    NodeContainer expression;
    NodeContainer true_case;
    NodeContainer false_case;;

    TernaryNode(NodeContainer expression, NodeContainer trueCase, NodeContainer falseCase);

    void accept(Visitor& visitor) override;

    bool equal(NodeContainer other) const override;

    bool operator==(const TernaryNode& other) const;

};


#endif //TERNARYNODE_H
