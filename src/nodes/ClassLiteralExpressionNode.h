//
// Created by chris on 6/8/20.
//

#ifndef UNTITLED1_CLASSLITERALEXPRESSIONNODE_H
#define UNTITLED1_CLASSLITERALEXPRESSIONNODE_H


#include "Node.h"
#include "Visitor.h"
#include "TypeNode.h"

class ClassLiteralExpressionNode : public Node {

public:
    ClassLiteralExpressionNode(ObjectTypeNode* type, const std::vector<Node*>& init);

    std::vector<Node*> init;
    std::vector<std::string> names;
    ObjectTypeNode* type;
private:
    void accept(Visitor& visitor) override;

    bool equal(Node* other) const override;

};


#endif //UNTITLED1_CLASSLITERALEXPRESSIONNODE_H
