//
// Created by chris on 6/8/20.
//

#ifndef CLASSLITERALEXPRESSIONNODE_H
#define CLASSLITERALEXPRESSIONNODE_H


#include "Node.h"

#include "TypeNode.h"
#include "Node.h"

class ClassLiteralExpressionNode : public Node {

public:
    ClassLiteralExpressionNode(ObjectTypeNode* type, const VectorOfNodes& init);

    ~ClassLiteralExpressionNode();

    VectorOfNodes init;
    std::vector<std::string> names;
    ObjectTypeNode* type;

    bool equal(const Node& x) const override;
    ClassLiteralExpressionNode& clsexp() override;
    const ClassLiteralExpressionNode& clsexp() const override;
};


#endif //CLASSLITERALEXPRESSIONNODE_H
