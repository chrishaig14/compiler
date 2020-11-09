//
// Created by chris on 6/8/20.
//

#ifndef UNTITLED1_CLASSLITERALFIELDNODE_H
#define UNTITLED1_CLASSLITERALFIELDNODE_H


#include "Node.h"
#include "Visitor.h"
#include "TypeNode.h"

class ClassLiteralFieldNode : public Node {
public:
    ObjectTypeNode* type;
    std::map<std::string, NodeContainer> init;

    ClassLiteralFieldNode(ObjectTypeNode* type, const std::map<std::string, NodeContainer>& init);

    void accept(Visitor& visitor) override;

    bool equal(NodeContainer other) const override;

};


#endif //UNTITLED1_CLASSLITERALFIELDNODE_H
