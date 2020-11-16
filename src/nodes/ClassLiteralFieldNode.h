//
// Created by chris on 6/8/20.
//

#ifndef CLASSLITERALFIELDNODE_H
#define CLASSLITERALFIELDNODE_H


#include "Node.h"
#include "Visitor.h"
#include "TypeNode.h"

class ClassLiteralFieldNode : public Node {
public:
    ObjectTypeNode& type;

    bool operator==(const ClassLiteralFieldNode& other) const;

    bool operator!=(const ClassLiteralFieldNode& other) const;

    std::map<std::string, NodeContainer> init;

    ClassLiteralFieldNode(ObjectTypeNode& type, const std::map<std::string, NodeContainer>& init);

    void accept(Visitor& visitor) override;

};


#endif //CLASSLITERALFIELDNODE_H
