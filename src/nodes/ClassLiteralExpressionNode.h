//
// Created by chris on 6/8/20.
//

#ifndef CLASSLITERALEXPRESSIONNODE_H
#define CLASSLITERALEXPRESSIONNODE_H


#include "Node.h"
#include "Visitor.h"
#include "TypeNode.h"
#include "Node.h"

class ClassLiteralExpressionNode : public Node {

public:
    ClassLiteralExpressionNode(ObjectTypeNode* type, const VectorOfNodes& init);

    VectorOfNodes init;
    std::vector<std::string> names;
    ObjectTypeNode* type;

    void accept(Visitor& visitor) override;

    bool equal(const Node& x) const override;
};


#endif //CLASSLITERALEXPRESSIONNODE_H
