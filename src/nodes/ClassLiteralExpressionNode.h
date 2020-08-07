//
// Created by chris on 6/8/20.
//

#ifndef UNTITLED1_CLASSLITERALEXPRESSIONNODE_H
#define UNTITLED1_CLASSLITERALEXPRESSIONNODE_H


#include "Node.h"
#include "Visitor.h"

class ClassLiteralExpressionNode : public Node {
    std::string identifier;
    std::vector<Node*> init;

public:
    ClassLiteralExpressionNode(const std::string& identifier, const std::vector<Node*>& init);

private:
    void accept(Visitor& visitor) override;

    bool equal(Node* other) const override;

};


#endif //UNTITLED1_CLASSLITERALEXPRESSIONNODE_H
