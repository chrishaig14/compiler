//
// Created by chris on 6/8/20.
//

#ifndef CLASSLITERALEXPRESSIONNODE_H
#define CLASSLITERALEXPRESSIONNODE_H


#include "Node.h"
#include "Visitor.h"
#include "TypeNode.h"
#include "NodeContainer.h"

class ClassLiteralExpressionNode : public Node {

public:
    ClassLiteralExpressionNode(ObjectTypeNode* type, const std::vector<NodeContainer>& init);

    std::vector<NodeContainer> init;
    std::vector<std::string> names;
    ObjectTypeNode* type;

    void accept(Visitor& visitor) override;

private:

    bool equal(NodeContainer other) const override;

};


#endif //CLASSLITERALEXPRESSIONNODE_H
